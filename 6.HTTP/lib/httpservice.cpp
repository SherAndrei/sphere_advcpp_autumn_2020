#include "httpservice.h"
#include <algorithm>
#include "globallogger.h"
#include "httpconnection.h"
#include "tcperr.h"
#include "neterr.h"
#include "httperr.h"

namespace {

http::HttpConnection* get(tcp::IConnectable* p_conn) {
    return dynamic_cast<http::HttpConnection*>(p_conn);
}

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

HttpService::HttpService(const tcp::Address& addr, IHttpListener* listener, size_t workerSize,
    size_t connection_timeout_sec, size_t keep_alive_timeout_sec)
    : IService(addr)
    , listener_(listener)
    , conn_timeo(connection_timeout_sec)
    , ka_conn_timeo(keep_alive_timeout_sec) {
    server_.set_reuseaddr();
    server_.set_timeout(ACCEPT_TIMEOUT);
    workers_.reserve(std::min(static_cast<size_t>(std::thread::hardware_concurrency()),
                        workerSize));
    if (workers_.capacity() == 0u) {
        throw WorkerError("Workers size == 0");
    }
}

void HttpService::setListener(IHttpListener* listener) {
    listener_ = listener;
}

size_t HttpService::connections_size() {
    std::lock_guard<std::mutex> lock(closing_mutex_);
    return clients_.size() - closed_.size();
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

    while (server_.socket().valid()) {
        log::debug("Server waits");
        try {
            net::IClient* p_client = add_new_connection(server_.accept_non_block());
            log::info("Server accepts: " + p_client->conn->address().str());
            net::OPTION& client_opt = get(p_client->conn.get())->epoll_option_;
            client_opt = net::OPTION::READ + net::OPTION::ET_ONESHOT;
            epoll_.add(p_client, client_opt);
        } catch (tcp::TimeOutError& er) {
            // ignore
        } catch (net::EPollError& er) {
            log::error("Server encounters " + std::string(er.what()));
        }
        dump_timed_out_connections();
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
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        log::debug("Worker " + std::to_string(th_num) + " got " + std::to_string(epoll_events.size())
                                                      + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            net::IClient* p_client = static_cast<net::IClient*>(event.data.ptr);
            HttpConnection* p_conn = get(p_client->conn.get());

            if (!try_reset_last_activity_time(p_client))
                continue;

            if (event.events & EPOLLIN) {
                log::debug("Worker " + std::to_string(th_num) + " encounters EPOLLIN from "
                                                              + p_conn->address().str());

                if (!try_read_request(p_client, th_num))
                    continue;
                listener_->OnRequest(*p_conn);

                if (!p_conn->keep_alive_)
                    p_conn->unsubscribe(net::OPTION::READ);
                else
                    p_conn->read_.clear();
                p_conn->subscribe(net::OPTION::WRITE);
                log::info("Worker " + std::to_string(th_num) + " successfully read from "
                                                             + p_conn->address().str());
            } else if (event.events & EPOLLOUT) {
                log::debug("Worker " + std::to_string(th_num) + " encounters EPOLLOUT from "
                                                              + p_conn->address().str());
                if (try_write_responce(p_client))
                    p_conn->unsubscribe(net::OPTION::WRITE);
                log::info("Worker " + std::to_string(th_num) + " successfully wrote to "
                                                             + p_conn->address().str());
            }
            if (p_conn->epoll_option_ == net::OPTION::ET_ONESHOT) {
                close_client(p_client);
            } else {
                subscribe(p_client, net::OPTION::UNKNOWN);
            }
        }
    }
}

void HttpService::dump_timed_out_connections() {
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    while (!timeo_qu_.empty() && close_if_timed_out(timeo_qu_.front()))
        timeo_qu_.pop();
}

net::IClient* HttpService::add_new_connection(tcp::NonBlockConnection&& cn) {
    {
        std::lock_guard<std::mutex> lock(closing_mutex_);
        if (!closed_.empty()) {
            net::IClient* p_client = closed_.front();
            (*p_client).conn = std::make_unique<HttpConnection>(std::move(cn));
            closed_.pop();
            std::lock_guard<std::mutex> lock(timeout_mutex_);
            timeo_qu_.emplace(p_client);
            return p_client;
        }
    }
    clients_.push_back(net::IClient{std::make_unique<HttpConnection>(std::move(cn))});
    clients_.back().iter = std::prev(clients_.end());
    net::IClient* p_client = &(clients_.back());
    std::lock_guard<std::mutex> lock(timeout_mutex_);
    timeo_qu_.emplace(p_client);
    return p_client;
}

bool HttpService::try_write_responce(net::IClient* p_client) {
    HttpConnection* p_conn = get(p_client->conn.get());
    while (true) {
        try {
            p_conn->write_from_buffer();
            if (p_conn->write_buf().empty())
                return true;
        } catch (tcp::TimeOutError& ex) {
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_client);
            return false;
        }
    }
    return  false;
}

bool HttpService::try_read_request(net::IClient* p_client, size_t th_num) {
    HttpConnection* p_conn = get(p_client->conn.get());
    while (true) {
        try {
            if (p_conn->read_to_buffer() == 0) {
                log::info("Client " + p_conn->address().str() + " closed unexpectedly");
                close_client(p_client);
                return false;
            }
            Request req(p_conn->read_buf());
            p_conn->keep_alive_ = is_keep_alive(req.headers());
            p_conn->req_ = std::move(req);
        } catch (tcp::TimeOutError& ex) {
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_client);
            return false;
        } catch (ExpectingData& exd) {
            log::warn("Worker " + std::to_string(th_num) + " got incomplete request from "
                                                         + p_conn->address().str());
            subscribe(p_client, net::OPTION::READ);
            return false;
        } catch (IncorrectData& ind) {
            log::warn("Worker " + std::to_string(th_num) + " got incorrect request from "
                                                         + p_conn->address().str());
            close_client(p_client);
            return false;
        }
    }
    return true;
}

bool HttpService::try_reset_last_activity_time(net::IClient* p_client) {
    HttpConnection* p_conn = get(p_client->conn.get());
    std::lock_guard<std::mutex> lock(p_conn->timeout_mutex_);
    if (p_conn->socket().valid()) {
        p_conn->reset_time_of_last_activity();
        return true;
    }
    return false;
}

void HttpService::close() {
    server_.close();
}

void HttpService::subscribe(net::IClient* p_client, net::OPTION opt)   const {
    HttpConnection* p_conn = get(p_client->conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ + opt + net::OPTION::ET_ONESHOT;
    epoll_.mod(p_client, p_conn->epoll_option_);
}

void HttpService::unsubscribe(net::IClient* p_client, net::OPTION opt) const {
    HttpConnection* p_conn = get(p_client->conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ - opt + net::OPTION::ET_ONESHOT;
    epoll_.mod(p_client, p_conn->epoll_option_);
}

bool HttpService::close_if_timed_out(net::IClient* p_client) {
    HttpConnection* cn = get(p_client->conn.get());
    size_t timeo = (cn->keep_alive_ ? ka_conn_timeo : conn_timeo);
    std::scoped_lock lock(closing_mutex_, cn->timeout_mutex_);
    if (cn->socket().valid() && cn->is_timed_out(timeo)) {
        log::info("Connection timed out: " + cn->address().str());
        closed_.push(p_client);
        cn->close();
        return true;
    }
    return false;
}

void HttpService::close_client(net::IClient* client) {
    HttpConnection* cn = get(client->conn.get());
    std::lock_guard<std::mutex> lock(closing_mutex_);
    if (cn->socket().valid()) {
        log::info("Closing connection: " + cn->address().str());
        closed_.push(client);
        cn->close();
    }
}

}  // namespace http
