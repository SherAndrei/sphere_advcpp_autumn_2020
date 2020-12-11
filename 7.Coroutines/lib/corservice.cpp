#include "corservice.h"
#include "globallogger.h"
#include "corconnection.h"
#include "tcperr.h"
#include "httperr.h"
#include "neterr.h"

namespace {

http::cor::CorConnection* get(tcp::IConnection* p_conn) {
    return dynamic_cast<http::cor::CorConnection*>(p_conn);
}

bool is_keep_alive(const std::vector<http::Header>& headers) {
     auto it = std::find_if(headers.begin(), headers.end(),
                          [](const http::Header& h) {
                              return h.name == "Connection";
                          });
    if (it == headers.end()) {
        return false;
    }

    return it->value.find("Keep-Alive") != it->value.npos;
}

void shift_to_back(std::list<net::ConnectionAndData*>& cont,
                   std::list<net::ConnectionAndData*>::iterator iter) {
    cont.splice(cont.end(), cont, iter);
    cont.back()->timeout_iter = std::prev(cont.end());
}

}  // namespace


namespace http {
namespace cor  {

CoroutineService::CoroutineService(const tcp::Address& addr, ICoroutineListener* listener, size_t workersSize,
                     size_t connection_timeout_sec, size_t keep_alive_timeout_sec)
    : HttpService(addr, nullptr, workersSize, connection_timeout_sec, keep_alive_timeout_sec) {
    setListener(listener);
}

ICoroutineListener* CoroutineService::getListener() {
    return dynamic_cast<ICoroutineListener*>(listener_);
}
void CoroutineService::setListener(ICoroutineListener* listener) {
    listener_ = listener;
}

void CoroutineService::work(size_t th_num) {
    while (true) {
        log::debug("Worker " + std::to_string(th_num) + " waits");
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        log::debug("Worker " + std::to_string(th_num) + " got " + std::to_string(epoll_events.size())
                                                      + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            net::ConnectionAndData* p_place = static_cast<net::ConnectionAndData*>(event.data.ptr);
            cor::resume(get_routine(p_place));
        }
    }
}

Routine* CoroutineService::get_routine(net::ConnectionAndData* p_place) {
    std::lock_guard<std::mutex> lk(p_place->timeout_mutex);
    CorConnection* p_conn = get(p_place->u_conn.get());
    if (!p_conn->is_routine_set) {
        p_conn->set_routine(cor::create([this, p_place] { serve_client(p_place); }));
    }
    return p_conn->routine();
}

void CoroutineService::serve_client(net::ConnectionAndData* p_place) {
    CorConnection* p_conn = get_connection_and_try_reset_last_activity_time(p_place);
    if (!p_conn)
        return;

    if (!try_read_request(p_place))
        return;

    p_conn->write(getListener()->OnRequest(std::move(p_conn->req_)));
    p_conn->read_.clear();
    if (!p_conn->keep_alive_)
        unsubscribe(p_place, net::OPTION::READ);
    subscribe(p_place, net::OPTION::WRITE);

    log::info("Worker successfully read from " + p_conn->address().str());
    cor::yield();
    if (try_write_responce(p_place))
        unsubscribe(p_place, net::OPTION::WRITE);

    log::info("Worker successfully wrote to " + p_conn->address().str());
    if (p_conn->epoll_option_ == net::OPTION::ET_ONESHOT) {
        close_client(p_place);
    } else {
        get(p_place->u_conn.get())->set_routine(nullptr);
        subscribe(p_place, net::OPTION::UNKNOWN);
    }
}

net::ConnectionAndData* CoroutineService::emplace_connection(tcp::NonBlockConnection&& cn) {
    net::ConnectionAndData* p_place = try_replace_closed_with_new_conn(std::move(cn));
    if (p_place != nullptr) {
        return p_place;
    }
    connections_.emplace_back(std::make_unique<CorConnection>(std::move(cn), nullptr));
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    timeod_.emplace_back(&connections_.back());
    timeod_.back()->timeout_iter = std::prev(timeod_.end());
    return timeod_.back();
}

net::ConnectionAndData* CoroutineService::try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn) {
    std::lock_guard<std::mutex> lock_cl(closing_mutex_);
    if (closed_.empty()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    closed_.front()->u_conn = std::make_unique<CorConnection>(std::move(cn), nullptr);
    auto iter = closed_.front()->timeout_iter;
    closed_.pop();
    shift_to_back(timeod_, iter);
    return timeod_.back();
}


bool CoroutineService::try_read_request(net::ConnectionAndData* p_place) {
    CorConnection* p_conn = get(p_place->u_conn.get());
    while (true) {
        while (true) {
            try {
                if (p_conn->read_to_buffer() == 0) {
                    log::info("Client " + p_conn->address().str() + " closed unexpectedly");
                    close_client(p_place);
                    return false;
                }
            } catch (tcp::TimeOutError& ex) {
                break;
            } catch (tcp::DescriptorError& ex) {
                log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
                close_client(p_place);
                return false;
            }
        }
        try {
            Request req(p_conn->read_buf());
            p_conn->keep_alive_ = is_keep_alive(req.headers());
            p_conn->req_ = std::move(req);
            break;
        } catch (ExpectingData& exd) {
            log::warn("Worker got incomplete request from " + p_conn->address().str());
            subscribe(p_place, net::OPTION::READ);
            cor::yield();
            continue;
        } catch (IncorrectData& ind) {
            log::warn("Worker got incorrect request from " + p_conn->address().str());
            close_client(p_place);
            return false;
        }
    }

    return true;
}

bool CoroutineService::try_write_responce(net::ConnectionAndData* p_place) {
    CorConnection* p_conn = get(p_place->u_conn.get());
    while (true) {
        try {
            p_conn->write_from_buffer();
            if (p_conn->write_buf().empty()) {
                return true;
            }
        } catch (tcp::TimeOutError& ex) {
            if (!p_conn->write_buf().empty()) {
                log::debug("Worker yields on write");
                subscribe(p_place, net::OPTION::WRITE);
                cor::yield();
                continue;
            } else {
                break;
            }
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_place);
            return false;
        }
    }
    return  false;
}

CorConnection*
CoroutineService::get_connection_and_try_reset_last_activity_time(net::ConnectionAndData* p_place) {
    std::scoped_lock lock(timeout_mutex_, p_place->timeout_mutex);
    CorConnection* p_conn = get(p_place->u_conn.get());
    if (p_conn->socket().valid()) {
        p_conn->reset_time_of_last_activity();
        shift_to_back(timeod_, p_place->timeout_iter);
        return p_conn;
    }
    return nullptr;
}

void CoroutineService::activate_workers() {
    for (size_t i = 0; i < workers_.capacity(); i++) {
        workers_.emplace_back(&CoroutineService::work, this, i + 1);
        log::debug("Thread " + std::to_string(i + 1) + " up and running");
    }
}

void CoroutineService::subscribe(net::ConnectionAndData* p_place, net::OPTION opt)   const {
    CorConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ + opt + net::OPTION::ET_ONESHOT;
    epoll_.mod(p_place, p_conn->epoll_option_);
}

void CoroutineService::unsubscribe(net::ConnectionAndData* p_place, net::OPTION opt) const {
    CorConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ - opt + net::OPTION::ET_ONESHOT;
    epoll_.mod(p_place, p_conn->epoll_option_);
}

bool CoroutineService::close_if_timed_out(net::ConnectionAndData* p_place) {
    std::lock_guard<std::mutex> lock1(closing_mutex_);
    std::lock_guard<std::mutex> lock2(p_place->timeout_mutex);
    CorConnection* p_conn = get(p_place->u_conn.get());
    size_t timeo = (p_conn->keep_alive_ ? ka_conn_timeo : conn_timeo);
    if (p_conn->socket().valid() && p_conn->is_timed_out(timeo)) {
        log::info("Connection timed out: " + p_conn->address().str());
        closed_.push(p_place);
        p_conn->close();
        return true;
    }
    return false;
}

}  // namespace cor
}  // namespace http
