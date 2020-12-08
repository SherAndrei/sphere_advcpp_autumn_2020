#include "corservice.h"
#include "globallogger.h"
#include "corconnection.h"
#include "tcperr.h"
#include "httperr.h"

namespace {

http::cor::CorConnection* get(tcp::IConnectable* p_conn) {
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

void shift_to_back(net::ClientPlaces& cont, std::list<net::IClientPlace>::iterator iter) {
    cont.splice(cont.end(), cont, iter);
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
            net::IClientPlace* p_place = static_cast<net::IClientPlace*>(event.data.ptr);
            CorConnection* p_conn = get(p_place->p_client->conn.get());
            if (!p_conn->is_routine_set) {
                p_conn->set_routine(cor::create([this, p_place] { serve_client(p_place); }));
            }
            cor::resume(p_conn->routine_id());
        }
    }
}

void CoroutineService::serve_client(net::IClientPlace* p_place) {
    net::IClient* p_client = p_place->p_client;
    CorConnection* p_conn = get(p_client->conn.get());

    if (!try_reset_last_activity_time(p_place))
        return;

    log::debug("Worker encounters EPOLLIN from " + p_conn->address().str());

    if (!try_read_request(p_place))
        return;

    p_conn->write(getListener()->OnRequest(std::move(p_conn->req_)));
    p_conn->read_.clear();
    if (!p_conn->keep_alive_)
        unsubscribe(p_place, net::OPTION::READ);
    subscribe(p_place, net::OPTION::WRITE);

    log::info("Worker successfully read from " + p_conn->address().str());
    log::debug("Worker yields after epollin");
    cor::yield();
    log::debug("Worker encounters EPOLLOUT from " + p_conn->address().str());

    if (try_write_responce(p_place))
        unsubscribe(p_place, net::OPTION::WRITE);

    log::info("Worker successfully wrote to " + p_conn->address().str());

    if (p_conn->epoll_option_ == net::OPTION::ET_ONESHOT) {
        close_client(p_place);
    } else {
        subscribe(p_place, net::OPTION::UNKNOWN);
    }
    log::debug("Worker ends routine");
}

net::IClientPlace* CoroutineService::emplace_client(tcp::NonBlockConnection&& cn) {
    net::IClientPlace* p_place = try_replace_closed_with_new_conn(std::move(cn));
    if (p_place != nullptr) {
        return p_place;
    }
    clients_.emplace_back(net::IClient{std::make_unique<CorConnection>(std::move(cn), 0u)});
    net::IClientPlace place;
    place.p_client = &clients_.back();
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    timeod_.emplace_back(place);
    timeod_.back().iter = std::prev(timeod_.end());
    return &timeod_.back();
}

net::IClientPlace* CoroutineService::try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn) {
    std::lock_guard<std::mutex> lock_cl(closing_mutex_);
    if (closed_.empty()) {
        return nullptr;
    }
    net::IClientPlace place = closed_.front();
    place.p_client->conn = std::make_unique<CorConnection>(std::move(cn), 0u);
    auto iter = place.iter;
    closed_.pop();
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    shift_to_back(timeod_, iter);
    timeod_.back().iter = std::prev(timeod_.end());
    return &timeod_.back();
}


bool CoroutineService::try_read_request(net::IClientPlace* p_place) {
    CorConnection* p_conn = get(p_place->p_client->conn.get());
    while (true) {
        try {
            if (p_conn->read_to_buffer() == 0) {
                log::info("Client " + p_conn->address().str() + " closed unexpectedly");
                close_client(p_place);
                return false;
            }
            Request req(p_conn->read_buf());
            p_conn->keep_alive_ = is_keep_alive(req.headers());
            p_conn->req_ = std::move(req);
            break;
        } catch (tcp::TimeOutError& ex) {
            log::debug("Worker yields on read");
            cor::yield();
            continue;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_place);
            return false;
        } catch (ExpectingData& exd) {
            log::warn("Worker got incomplete request from " + p_conn->address().str());
            subscribe(p_place, net::OPTION::READ);
            return false;
        } catch (IncorrectData& ind) {
            log::warn("Worker got incorrect request from " + p_conn->address().str());
            close_client(p_place);
            return false;
        }
    }
    return true;
}

bool CoroutineService::try_write_responce(net::IClientPlace* p_place) {
    CorConnection* p_conn = get(p_place->p_client->conn.get());
    while (true) {
        try {
            p_conn->write_from_buffer();
            if (p_conn->write_buf().empty())
                return true;
        } catch (tcp::TimeOutError& ex) {
            log::debug("Worker yields on write");
            cor::yield();
            continue;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_place);
            return false;
        }
    }
    return  false;
}

void CoroutineService::activate_workers() {
    for (size_t i = 0; i < workers_.capacity(); i++) {
        workers_.emplace_back(&CoroutineService::work, this, i + 1);
        log::debug("Thread " + std::to_string(i + 1) + " up and running");
    }
}

}  // namespace cor
}  // namespace http
