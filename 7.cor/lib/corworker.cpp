#include "corworker.h"
#include <vector>
#include <algorithm>
#include "globallogger.h"
#include "corservice.h"
#include "corconnection.h"
#include "tcperr.h"
#include "httperr.h"
namespace {

http::cor::CorConnection* get(tcp::BaseConnection* p_conn) {
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

void CoroutineWorker::work() {
    while (true) {
        log::debug(info() + " waits");
        std::vector<::epoll_event> epoll_events = p_service_->epoll_.wait();
        log::debug(info() + " got " + std::to_string(epoll_events.size())
                                                      + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            net::ConnectionAndData* p_place = static_cast<net::ConnectionAndData*>(event.data.ptr);
            std::lock_guard lock(p_place->connection_mutex);
            CorConnection* p_conn = get(p_place->u_conn.get());
            if (!p_conn->routine()) {
                p_conn->set_routine(cor::create([this, p_place] { serve_client(p_place); }));
            }
            cor::resume(p_conn->routine());
        }
    }
}

void CoroutineWorker::serve_client(net::ConnectionAndData* p_place) {
    CorConnection* p_conn = get(p_place->u_conn.get());
    reset_last_activity_time(p_place);

    if (!try_read_request(p_place))
        return;

    p_conn->write(p_service_->getListener()->OnRequest(std::move(p_conn->req_)));
    p_conn->read_.clear();
    if (!p_conn->keep_alive_)
        unsubscribe(p_place, net::OPTION::READ);
    subscribe(p_place, net::OPTION::WRITE);

    log::info(info() + " successfully read from " + p_conn->address().str());
    cor::yield();
    if (try_write_responce(p_place))
        unsubscribe(p_place, net::OPTION::WRITE);

    log::info(info() + " successfully wrote to " + p_conn->address().str());
    if (p_conn->epoll_option_ == net::OPTION::ET_ONESHOT) {
        close_client(p_place);
    } else {
        p_conn->set_routine(nullptr);
        subscribe(p_place, net::OPTION::UNKNOWN);
    }
}

void CoroutineWorker::reset_last_activity_time(net::ConnectionAndData* p_place) {
    std::scoped_lock lock(p_service_->timeout_mutex_);
    ITimed* p_timed = dynamic_cast<ITimed*>(p_place->u_conn.get());
    p_timed->reset_time_of_last_activity();
    shift_to_back(p_service_->timeod_, p_place->timeout_iter);
}

void CoroutineWorker::close_client(net::ConnectionAndData* p_place) {
    tcp::BaseConnection* p_conn = p_place->u_conn.get();
    if (p_conn->socket().valid()) {
        log::info("Closing connection: " + p_conn->address().str());
        std::lock_guard<std::mutex> lock(p_service_->closing_mutex_);
        p_service_->closed_.push(p_place);
        p_conn->close();
    }
}

bool CoroutineWorker::try_read_request(net::ConnectionAndData* p_place) {
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
            log::warn("Worker got incomplete request from " + p_conn->address().str()
                      + ": " + exd.what());
            subscribe(p_place, net::OPTION::READ);
            cor::yield();
            continue;
        } catch (IncorrectData& ind) {
            log::warn("Worker got incorrect request from " + p_conn->address().str()
                      + ": " + ind.what());
            close_client(p_place);
            return false;
        }
    }

    return true;
}

bool CoroutineWorker::try_write_responce(net::ConnectionAndData* p_place) {
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

void CoroutineWorker::subscribe(net::ConnectionAndData* p_place, net::OPTION opt)   const {
    CorConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ + opt + net::OPTION::ET_ONESHOT;
    p_service_->epoll_.mod(p_place, p_conn->epoll_option_);
}

void CoroutineWorker::unsubscribe(net::ConnectionAndData* p_place, net::OPTION opt) const {
    CorConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ - opt + net::OPTION::ET_ONESHOT;
    p_service_->epoll_.mod(p_place, p_conn->epoll_option_);
}

void CoroutineWorker::set_service_pointer(CoroutineService* p_service) {
    p_service_ = p_service;
}

}  // namespace cor
}  // namespace http
