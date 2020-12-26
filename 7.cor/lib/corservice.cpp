#include "corservice.h"
#include "globallogger.h"
#include "corconnection.h"
#include "corworker.h"
#include "tcperr.h"
#include "httperr.h"
#include "neterr.h"

namespace {

http::cor::CorConnection* get(tcp::BaseConnection* p_conn) {
    return dynamic_cast<http::cor::CorConnection*>(p_conn);
}

}  // namespace


namespace http {
namespace cor  {

CoroutineService::CoroutineService(const tcp::Address& addr, ICoroutineListener* listener, size_t workersSize,
                     size_t connection_timeout_sec, size_t keep_alive_timeout_sec)
    : HttpService(addr, nullptr, 0ul, connection_timeout_sec, keep_alive_timeout_sec) {
    workers_.resize(std::min(static_cast<size_t>(std::thread::hardware_concurrency()), workersSize));
    setListener(listener);
}

ICoroutineListener* CoroutineService::getListener() {
    return dynamic_cast<ICoroutineListener*>(listener_);
}
void CoroutineService::setListener(ICoroutineListener* listener) {
    listener_ = listener;
}

void CoroutineService::activate_workers() {
    for (size_t i = 0; i < workers_.size(); i++) {
        workers_[i].set_service_pointer(this);
        workers_[i].set_thread_num(i);
        workers_[i].set_thread(std::thread(&CoroutineWorker::work, &workers_[i]));
        log::debug(workers_[i].info() + " up and running");
    }
}

void CoroutineService::run() {
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


net::ConnectionAndData* CoroutineService::emplace_connection(tcp::NonBlockConnection&& cn) {
    net::ConnectionAndData* p_place = try_replace_closed_with_new_conn(std::move(cn));
    if (p_place != nullptr) {
        return p_place;
    }
    connections_.emplace_back(std::make_unique<CorConnection>(std::move(cn), nullptr));
    std::lock_guard lock_to(timeout_mutex_);
    timeod_.emplace_back(&connections_.back());
    timeod_.back()->timeout_iter = std::prev(timeod_.end());
    return timeod_.back();
}

net::ConnectionAndData* CoroutineService::try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn) {
    std::lock_guard lock_cl(closing_mutex_);
    if (closed_.empty()) {
        return nullptr;
    }
    net::ConnectionAndData* p_place = closed_.front();
    p_place->u_conn = std::make_unique<CorConnection>(std::move(cn), nullptr);
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

bool CoroutineService::close_if_timed_out(net::ConnectionAndData* p_place) {
    std::lock_guard lc(p_place->connection_mutex);
    CorConnection* p_conn = get(p_place->u_conn.get());
    size_t timeo = (p_conn->keep_alive_ ? ka_conn_timeo : conn_timeo);
    if (p_conn->socket().valid() && p_conn->is_timed_out(timeo)) {
        log::info("Connection timed out: " + p_conn->address().str());
        std::lock_guard lock1(closing_mutex_);
        closed_.push(p_place);
        p_conn->close();
        return true;
    }
    return false;
}

}  // namespace cor
}  // namespace http
