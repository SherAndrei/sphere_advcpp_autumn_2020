#include "httpservice.h"
#include <algorithm>
#include "globallogger.h"
#include "httpconnection.h"
#include "tcperr.h"
#include "neterr.h"
#include "httperr.h"

namespace {

http::HttpConnection* get(tcp::IConnection* p_conn) {
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

    return it->value.find("Keep-Alive") != it->value.npos;
}

void shift_to_back(std::list<net::ConnectionAndData*>& cont,
                   std::list<net::ConnectionAndData*>::iterator iter) {
    cont.splice(cont.end(), cont, iter);
    cont.back()->timeout_iter = std::prev(cont.end());
}

}  // namespace

namespace http {

HttpService::HttpService(const tcp::Address& addr, IHttpListener* listener, size_t workerSize,
    size_t connection_timeout_sec, size_t keep_alive_timeout_sec)
    : IService(addr, listener)
    , conn_timeo(connection_timeout_sec)
    , ka_conn_timeo(keep_alive_timeout_sec) {
    server_.socket().set_reuseaddr();
    server_.socket().set_rcvtimeo(ACCEPT_TIMEOUT);
    workers_.reserve(std::min(static_cast<size_t>(std::thread::hardware_concurrency()),
                             workerSize));
    if (workers_.capacity() == 0u) {
        throw WorkerError("Workers size == 0");
    }
}

void HttpService::setListener(IHttpListener* listener) {
    listener_ = listener;
}

IHttpListener* HttpService::getListener() {
    return dynamic_cast<IHttpListener*>(listener_);
}

size_t HttpService::connections_size() {
    std::lock_guard<std::mutex> lock(closing_mutex_);
    return connections_.size() - closed_.size();
}

void HttpService::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.socket().set_reuseaddr();
    t_serv.socket().set_rcvtimeo(ACCEPT_TIMEOUT);
    server_ = std::move(t_serv);
    log::info("Server " + server_.address().str() +  " succesfully opened");
}

void HttpService::activate_workers() {
    for (size_t i = 0; i < workers_.capacity(); i++) {
        workers_.emplace_back(&HttpService::work, this, i + 1);
        log::debug("Thread " + std::to_string(i + 1) + " up and running");
    }
}

void HttpService::run() {
    if (listener_ == nullptr)
        throw net::ListenerError("Listener was not set");

    activate_workers();

    while (server_.socket().valid()) {
        log::debug("Server waits");
        try {
            net::ConnectionAndData* p_place = emplace_connection(server_.accept_non_block());
            log::info("Server accepts: " + p_place->u_conn->address().str());
            net::OPTION& client_opt = get(p_place->u_conn.get())->epoll_option_;
            client_opt = net::OPTION::READ + net::OPTION::ET_ONESHOT;
            epoll_.add(p_place, client_opt);
        } catch (tcp::TimeOutError& er) {
            log::debug("Server accepts nothing");
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
            net::ConnectionAndData* p_place = static_cast<net::ConnectionAndData*>(event.data.ptr);
            HttpConnection* p_conn = get(p_place->u_conn.get());

            if (!try_reset_last_activity_time(p_place))
                continue;

            if (event.events & EPOLLIN) {
                log::debug("Worker " + std::to_string(th_num) + " encounters EPOLLIN from "
                                                              + p_conn->address().str());

                if (!try_read_request(p_place))
                    continue;

                getListener()->OnRequest(*p_conn);

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
                if (try_write_responce(p_place))
                    p_conn->unsubscribe(net::OPTION::WRITE);
                log::info("Worker " + std::to_string(th_num) + " successfully wrote to "
                                                             + p_conn->address().str());
            }
            if (p_conn->epoll_option_ == net::OPTION::ET_ONESHOT) {
                close_client(p_place);
            } else {
                subscribe(p_place, net::OPTION::UNKNOWN);
            }
        }
    }
}

void HttpService::dump_timed_out_connections() {
    std::lock_guard<std::mutex> lock(timeout_mutex_);
    while (timeod_.size() > 0 && close_if_timed_out(timeod_.front())) {
        timeod_.pop_front();
    }
}

net::ConnectionAndData* HttpService::emplace_connection(tcp::NonBlockConnection&& cn) {
    net::ConnectionAndData* p_place = try_replace_closed_with_new_conn(std::move(cn));
    if (p_place != nullptr) {
        return p_place;
    }
    connections_.emplace_back(std::make_unique<HttpConnection>(std::move(cn)));
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    timeod_.emplace_back(&connections_.back());
    timeod_.back()->timeout_iter = std::prev(timeod_.end());
    return timeod_.back();
}

net::ConnectionAndData* HttpService::try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn) {
    std::lock_guard<std::mutex> lock_cl(closing_mutex_);
    if (closed_.empty()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock_to(timeout_mutex_);
    closed_.front()->u_conn = std::make_unique<HttpConnection>(std::move(cn));
    auto iter = closed_.front()->timeout_iter;
    closed_.pop();
    shift_to_back(timeod_, iter);
    return timeod_.back();
}

bool HttpService::try_write_responce(net::ConnectionAndData* p_place) {
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

bool HttpService::try_read_request(net::ConnectionAndData* p_place) {
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

bool HttpService::try_reset_last_activity_time(net::ConnectionAndData* p_place) {
    std::scoped_lock lock(timeout_mutex_, p_place->timeout_mutex);
    HttpConnection* p_timed = dynamic_cast<HttpConnection*>(get(p_place->u_conn.get()));
    if (p_place->u_conn->socket().valid()) {
        p_timed->reset_time_of_last_activity();
        shift_to_back(timeod_, p_place->timeout_iter);
        return true;
    }
    return false;
}

void HttpService::close() {
    server_.close();
}

void HttpService::subscribe(net::ConnectionAndData* p_place, net::OPTION opt)   const {
    HttpConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ + opt + net::OPTION::ET_ONESHOT;
    epoll_.mod(p_place, p_conn->epoll_option_);
}

void HttpService::unsubscribe(net::ConnectionAndData* p_place, net::OPTION opt) const {
    HttpConnection* p_conn = get(p_place->u_conn.get());
    p_conn->epoll_option_ = p_conn->epoll_option_ - opt + net::OPTION::ET_ONESHOT;
    epoll_.mod(p_place, p_conn->epoll_option_);
}

bool HttpService::close_if_timed_out(net::ConnectionAndData* p_place) {
    HttpConnection* p_conn = get(p_place->u_conn.get());
    size_t timeo = (p_conn->keep_alive_ ? ka_conn_timeo : conn_timeo);
    std::scoped_lock lock(closing_mutex_, p_place->timeout_mutex);
    if (p_conn->socket().valid() && p_conn->is_timed_out(timeo)) {
        log::info("Connection timed out: " + p_conn->address().str());
        closed_.push(p_place);
        p_conn->close();
        return true;
    }
    return false;
}

void HttpService::close_client(net::ConnectionAndData* p_place) {
    tcp::IConnection* p_conn = p_place->u_conn.get();
    std::lock_guard<std::mutex> lock(closing_mutex_);
    if (p_conn->socket().valid()) {
        log::info("Closing connection: " + p_conn->address().str());
        closed_.push(p_place);
        p_conn->close();
    }
}

}  // namespace http
