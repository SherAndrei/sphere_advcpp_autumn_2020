#include "httpworker.h"
#include <vector>
#include <algorithm>
#include "globallogger.h"
#include "httpservice.h"
#include "httpconnection.h"
#include "httpcontainers.h"
#include "tcperr.h"
#include "httperr.h"

namespace {

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

http::HttpConnection* get(tcp::IConnection* p_conn) {
    return dynamic_cast<http::HttpConnection*>(p_conn);
}

void shift_to_back(std::list<net::ConnectionAndData*>& cont,
                   std::list<net::ConnectionAndData*>::iterator iter) {
    cont.splice(cont.end(), cont, iter);
    cont.back()->timeout_iter = std::prev(cont.end());
}

}  // namespace

namespace http {

void HttpWorker::work() {
        while (true) {
        log::debug(info() + " waits");
        std::vector<::epoll_event> epoll_events = p_service_->epoll_.wait();
        log::debug(info() + " got " + std::to_string(epoll_events.size())
                                                      + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            net::ConnectionAndData* p_place = static_cast<net::ConnectionAndData*>(event.data.ptr);
            std::lock_guard lock(p_place->connection_mutex);
            HttpConnection* p_conn = get(p_place->u_conn.get());
            if (!p_conn->socket().valid()) {
                continue;
            }

            reset_last_activity_time(p_place);

            if (event.events & EPOLLIN) {
                log::debug(info() + " encounters EPOLLIN from " + p_conn->address().str());

                if (!try_read_request(p_place))
                    continue;

                p_service_->getListener()->OnRequest(*p_conn);

                if (!p_conn->keep_alive_)
                    p_conn->unsubscribe(net::OPTION::READ);
                else
                    p_conn->read_.clear();
                p_conn->subscribe(net::OPTION::WRITE);
                log::info(info() + " successfully read from " + p_conn->address().str());
            } else if (event.events & EPOLLOUT) {
                log::debug(info() + " encounters EPOLLOUT from " + p_conn->address().str());
                if (try_write_responce(p_place))
                    p_conn->unsubscribe(net::OPTION::WRITE);
                log::info(info() + " successfully wrote to " + p_conn->address().str());
            }
            if (p_conn->epoll_option_ == net::OPTION::ET_ONESHOT) {
                close_client(p_place);
            } else {
                subscribe(p_place, net::OPTION::UNKNOWN);
            }
        }
    }
}

void HttpWorker::reset_last_activity_time(net::ConnectionAndData* p_place) {
    std::scoped_lock lock(p_service_->timeout_mutex_);
    ITimed* p_timed = dynamic_cast<ITimed*>(p_place->u_conn.get());
    p_timed->reset_time_of_last_activity();
    shift_to_back(p_service_->timeod_, p_place->timeout_iter);
}

std::string HttpWorker::info() const {
    return "Worker " + std::to_string(thread_num_);
}

bool HttpWorker::try_write_responce(net::ConnectionAndData* p_place) {
    HttpConnection* p_conn = get(p_place->u_conn.get());
    while (true) {
        try {
            p_conn->write_from_buffer();
            if (p_conn->write_buf().empty())
                return true;
        } catch (tcp::TimeOutError& ex) {
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_place);
            return false;
        }
    }
    return  false;
}

bool HttpWorker::try_read_request(net::ConnectionAndData* p_place) {
    HttpConnection* p_conn = get(p_place->u_conn.get());
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
    } catch (ExpectingData& exd) {
        log::warn("Worker got incomplete request from " + p_conn->address().str());
        subscribe(p_place, net::OPTION::READ);
        return false;
    } catch (IncorrectData& ind) {
        log::warn("Worker got incorrect request from " + p_conn->address().str());
        close_client(p_place);
        return false;
    }

    return true;
}

void HttpWorker::subscribe(net::ConnectionAndData* p_place, net::OPTION opt)   const {
    HttpConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ + opt + net::OPTION::ET_ONESHOT;
    p_service_->epoll_.mod(p_place, p_conn->epoll_option_);
}

void HttpWorker::unsubscribe(net::ConnectionAndData* p_place, net::OPTION opt) const {
    HttpConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ - opt + net::OPTION::ET_ONESHOT;
    p_service_->epoll_.mod(p_place, p_conn->epoll_option_);
}

void HttpWorker::close_client(net::ConnectionAndData* p_place) {
    tcp::IConnection* p_conn = p_place->u_conn.get();
    if (p_conn->socket().valid()) {
        log::info("Closing connection: " + p_conn->address().str());
        std::lock_guard<std::mutex> lock(p_service_->closing_mutex_);
        p_service_->closed_.push(p_place);
        p_conn->close();
    }
}

void HttpWorker::set_thread(std::thread&& thread) {
    thread_ = std::move(thread);
}
void HttpWorker::set_thread_num(size_t thread_num) {
    thread_num_ = thread_num;
}
void HttpWorker::set_service_pointer(HttpService* p_service) {
    p_service_ = p_service;
}

void HttpWorker::join() {
    thread_.join();
}

}  // namespace http
