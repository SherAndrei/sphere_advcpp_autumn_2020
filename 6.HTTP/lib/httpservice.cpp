#include "httpservice.h"
#include "globallogger.h"
#include "httpconnection.h"
#include "tcperr.h"
#include "httperr.h"
#include "neterr.h"
#include "httpworker.h"

namespace {

http::HttpConnection* get(tcp::BaseConnection* p_conn) {
    return dynamic_cast<http::HttpConnection*>(p_conn);
}

}  // namespace

namespace http {

HttpService::HttpService(const tcp::Address& addr, IHttpListener* listener, size_t workerSize,
    size_t connection_timeout_sec, size_t keep_alive_timeout_sec)
    : BaseService(addr, listener)
    , conn_timeo(connection_timeout_sec)
    , ka_conn_timeo(keep_alive_timeout_sec) {
    server_.socket().set_reuseaddr();
    server_.socket().set_rcvtimeo(ACCEPT_TIMEOUT);
    workers_.resize(std::min(static_cast<size_t>(std::thread::hardware_concurrency()), workerSize));
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
    if (workers_.size() == 0ul) {
        throw WorkerError("Workers size == 0");
    }
    for (size_t i = 0; i < workers_.size(); i++) {
        workers_[i].set_service_pointer(this);
        workers_[i].set_thread_num(i);
        workers_[i].set_thread(std::thread(&HttpWorker::work, &workers_[i]));
        log::debug(workers_[i].info() + " up and running");
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

void HttpService::dump_timed_out_connections() {
    net::ConnectionAndData* p_place = nullptr;
    while (true) {
        {
            std::lock_guard lock(timeout_mutex_);
            if (timeod_.size() > 0)
                p_place = timeod_.front();
            else
                break;
        }
        if (close_if_timed_out(p_place)) {
            std::lock_guard lock(timeout_mutex_);
            timeod_.pop_front();
        } else {
            break;
        }
    }
}

net::ConnectionAndData* HttpService::emplace_connection(tcp::NonBlockConnection&& cn) {
    net::ConnectionAndData* p_place = try_replace_closed_with_new_conn(std::move(cn));
    if (p_place != nullptr) {
        return p_place;
    }
    connections_.emplace_back(std::make_unique<HttpConnection>(std::move(cn)));
    std::lock_guard lock_to(timeout_mutex_);
    timeod_.emplace_back(&connections_.back());
    timeod_.back()->timeout_iter = std::prev(timeod_.end());
    return timeod_.back();
}

net::ConnectionAndData* HttpService::try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn) {
    std::lock_guard lock_cl(closing_mutex_);
    if (closed_.empty()) {
        return nullptr;
    }
    net::ConnectionAndData* p_place = closed_.front();
    p_place->u_conn = std::make_unique<HttpConnection>(std::move(cn));
    closed_.pop();

    std::lock_guard lock_to(timeout_mutex_);
    if (timeod_.size() > 0) {
        timeod_.splice(timeod_.end(), timeod_, p_place->timeout_iter);
    } else {
        timeod_.emplace_back(p_place);
    }
    p_place->timeout_iter = std::prev(timeod_.end());
    return p_place;
}

void HttpService::close() {
    server_.close();
}

bool HttpService::close_if_timed_out(net::ConnectionAndData* p_place) {
    std::lock_guard lc(p_place->connection_mutex);
    HttpConnection* p_conn = get(p_place->u_conn.get());
    size_t timeo = (p_conn->keep_alive_ ? ka_conn_timeo : conn_timeo);
    if (p_conn->socket().valid() && p_conn->is_timed_out(timeo)) {
        log::info("Connection timed out: " + p_conn->address().str());
        std::lock_guard lock1(closing_mutex_);
        p_conn->close();
        closed_.push(p_place);
        return true;
    }
    return false;
}

}  // namespace http
