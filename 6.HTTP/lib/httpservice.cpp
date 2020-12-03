#include <algorithm>
#include "globallogger.h"
#include "httpservice.h"
#include "option.h"
#include "tcperr.h"
#include "neterr.h"
#include "httperr.h"
#include "connection.h"

namespace {

bool is_keep_alive(const std::vector<http::Header>& headers) {
     auto it = std::find_if(headers.begin(), headers.end(),
                          [](const http::Header& h) {
                              return h.name == "Connection";
                          });
    if (it == headers.end()) {
        return false;
    }

    return it->value.find("keep-alive") != it->value.npos;
}

}  // namespace

namespace http {

HttpService::HttpService(IHttpListener* listener, size_t workerSize) {
    workers_.reserve(std::min(static_cast<size_t>(std::thread::hardware_concurrency()),
                        workerSize));
    if (workers_.capacity() == 0u) {
        throw WorkerError("Workers size == 0");
    }
    setListener(listener);
}

void HttpService::setListener(IHttpListener* listener) {
    listener_ = listener;
}

size_t HttpService::connections_size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return manager_.size() - closed_.size();
}

void HttpService::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_reuseaddr();
    t_serv.set_timeout(ACCEPT_TIMEOUT);
    server_ = std::move(t_serv);
    log::info("Server " + server_.address().str() +  " succesfully opened");
}

void HttpService::run() {
    if (listener_ == nullptr)
        throw net::ListenerError("Listener was not set");

    for (size_t i = 0; i < workers_.capacity(); i++) {
        workers_.emplace_back(&HttpService::work, this, i + 1);
        log::debug("Thread " + std::to_string(i + 1) + " up and running");
    }

    HttpConnection* p_client = nullptr;
    while (server_.socket().valid()) {
        log::debug("Server waits");
        tcp::Connection new_c;
        try {
            new_c = server_.accept_non_block();
        } catch (tcp::TimeOutError& er) {
            log::debug("Server dropping coonnections");
            for (auto& connection : manager_) {
                close_if_timed_out(&connection);
            }
            log::info("Active connections: " + std::to_string(connections_size()));
            continue;
        }
        log::info("Server accepts: " + new_c.address().str());

        if (!try_replace_closed_with_new_connection(p_client, std::move(new_c))) {
            manager_.emplace_back(std::move(new_c));
            p_client = &(manager_.back());
        }

        p_client->epoll_option_ = net::OPTION::READ + net::OPTION::ET_ONESHOT;
        connection_epoll_.add(p_client, p_client->epoll_option_);
        log::info("Active connections: " + std::to_string(connections_size()));
    }

    for (size_t i = 0; i < workers_.size(); i++) {
        workers_[i].join();
        log::debug("Thread " + std::to_string(i + 1) + " finished");
    }
    log::info("Server finished");
}

void HttpService::work(size_t th_num) {
    while (true) {
        log::debug("Worker " + std::to_string(th_num) + " waits");
        std::vector<::epoll_event> epoll_events = connection_epoll_.wait();
        log::debug("Worker " + std::to_string(th_num)
                             + " got " + std::to_string(epoll_events.size())
                             + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            HttpConnection* p_client = static_cast<HttpConnection*>(event.data.ptr);
            mutex_.lock();
            if (!p_client->is_timed_out()) {
                p_client->reset_time_of_last_activity();
            }
            mutex_.unlock();
            if (event.events & EPOLLIN) {
                log::debug("Worker " + std::to_string(th_num)
                                     + " encounters EPOLLIN from " + p_client->address().str());

                if (!try_read_request(p_client, th_num))
                    continue;
                listener_->OnRequest(*p_client);

                if (!p_client->keep_alive_)
                    p_client->unsubscribe(net::OPTION::READ);
                else
                    p_client->read_.clear();
                p_client->subscribe(net::OPTION::WRITE);
                log::info("Worker " + std::to_string(th_num)
                                    + " successfully read from "
                                    + p_client->address().str());
            } else if (event.events & EPOLLOUT) {
                log::debug("Worker " + std::to_string(th_num)
                                     + " encounters EPOLLOUT from "
                                     + p_client->address().str());
                if (try_write_responce(p_client))
                    p_client->unsubscribe(net::OPTION::WRITE);
                log::info("Worker " + std::to_string(th_num)
                                    + " successfully wrote to "
                                    + p_client->address().str());
            }
            if (p_client->epoll_option_ == net::OPTION::ET_ONESHOT) {
                close_connection(p_client);
            } else {
                subscribe(*p_client, net::OPTION::UNKNOWN);
            }
        }
    }
}

bool HttpService::try_replace_closed_with_new_connection(HttpConnection*& p_client, tcp::Connection&& cn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!closed_.empty()) {
        p_client = closed_.front();
        *p_client = HttpConnection(std::move(cn));

        closed_.pop();
        return true;
    }
    return false;
}

bool HttpService::try_write_responce(HttpConnection* p_client) {
    while (true) {
        try {
            if (p_client->write_from_buffer() == 0u)
                break;
        } catch (tcp::TimeOutError& ex) {
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_client->address().str());
            close_connection(p_client);
            return false;
        }
    }

    return p_client->write_buf().empty();
}

bool HttpService::try_read_request(HttpConnection* p_client, size_t th_num) {
    while (true) {
        try {
            if (p_client->read_to_buffer() == 0) {
                log::info("Client " + p_client->address().str() + " closed unexpectedly");
                close_connection(p_client);
                return false;
            }
        } catch (tcp::TimeOutError& ex) {
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_client->address().str());
            close_connection(p_client);
            return false;
        }
    }
    try {
        Request req(p_client->read_buf());
        p_client->keep_alive_ = is_keep_alive(req.headers());
        p_client->req_ = std::move(req);
    } catch (ExpectingData& exd) {
        log::warn("Worker " + std::to_string(th_num)
                            + " got incomplete request from "
                            + p_client->address().str());
        subscribe(*p_client, net::OPTION::READ);
        return false;
    } catch (IncorrectData& ind) {
        log::warn("Worker " + std::to_string(th_num)
                            + " got incorrect request from "
                            + p_client->address().str());
        close_connection(p_client);
        return false;
    }
    return true;
}

void HttpService::close() {
    server_.close();
}

void HttpService::subscribe(HttpConnection& cn, net::OPTION opt)   const {
    cn.epoll_option_ = cn.epoll_option_ + opt + net::OPTION::ET_ONESHOT;
    connection_epoll_.mod(&(cn), cn.epoll_option_);
}

void HttpService::unsubscribe(HttpConnection& cn, net::OPTION opt) const {
    cn.epoll_option_ = cn.epoll_option_ - opt + net::OPTION::ET_ONESHOT;
    connection_epoll_.mod(&(cn), cn.epoll_option_);
}

void HttpService::close_if_timed_out(HttpConnection* cn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (cn->socket().valid() && cn->is_timed_out()) {
        log::info("Connection timed out: " + cn->address().str());
        closed_.push(cn);
        cn->close();
    }
}

void HttpService::close_connection(HttpConnection* cn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (cn->socket().valid()) {
        log::info("Closing connection: " + cn->address().str());
        closed_.push(cn);
        cn->close();
    }
}

}  // namespace http
