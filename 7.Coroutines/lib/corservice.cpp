#include "globallogger.h"
#include "neterr.h"
#include "httperr.h"
#include "tcperr.h"
#include "coroutine.h"
#include "corservice.h"

namespace http {
namespace cor {

CoroutineService::CoroutineService(ICoroutineListener* listener, size_t workerSize) {
    workers_.reserve(std::min(static_cast<size_t>(std::thread::hardware_concurrency()),
                        workerSize));
    if (workers_.capacity() == 0u) {
        throw WorkerError("Workers size == 0");
    }
    setListener(listener);
}

void CoroutineService::run() {
    if (listener_ == nullptr)
        throw net::ListenerError("Listener was not set");

    for (size_t i = 0; i < workers_.capacity(); i++) {
        workers_.emplace_back(&CoroutineService::work, this, i + 1);
        log::debug("Thread " + std::to_string(i + 1) + " up and running");
    }

    while (server_.socket().valid()) {
        log::debug("Server waits");
        CorConnection new_c;
        try {
            new_c = CorConnection(server_.accept_non_block(),
                                  cor::create(handle_client));
        } catch (tcp::TimeOutError& er) {
            log::debug("Server dropping coonnections");
            for (auto& connection : manager_) {
                close_if_timed_out(&connection);
            }
            log::info("Active connections: " + std::to_string(connections_size()));
            continue;
        }
        log::info("Server accepts: " + new_c.address().str());
        CorConnection* p_client = try_replace_closed_with_new_connection(std::move(new_c));
        if (p_client == nullptr) {
            manager_.emplace_back(std::move(new_c));
            p_client = &(manager_.back());
        }
        p_client->epoll_option_ = net::OPTION::READ + net::OPTION::ET_ONESHOT;
        epoll_.add(p_client, p_client->epoll_option_);
        log::info("Active connections: " + std::to_string(connections_size()));
    }
    for (size_t i = 0; i < workers_.size(); i++) {
        workers_[i].join();
        log::debug("Thread " + std::to_string(i + 1) + " finished");
    }
    log::info("Server finished");
}

void CoroutineService::handle_client(CorConnection* p_client) {

}

void CoroutineService::work(size_t th_num) {
    while (true) {
        log::debug("Worker " + std::to_string(th_num) + " waits");
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        log::debug("Worker " + std::to_string(th_num)
                             + " got " + std::to_string(epoll_events.size())
                             + " new epoll events");
        for (::epoll_event& event : epoll_events) {
            CorConnection* p_client = static_cast<CorConnection*>(event.data.ptr);

            if (!try_reset_last_activity_time(p_client))
                continue;

            if (event.events & EPOLLIN) {
                log::debug("Worker " + std::to_string(th_num)
                                     + " encounters EPOLLIN from " + p_client->address().str());

                if (!try_read_request(p_client, th_num))
                    continue;
                listener_->OnRequest(std::move(p_client->req_));

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

CorConnection* try_replace_closed_with_new_connection(CorConnection&& cn) {

}

}  // namespace cor
}  // namespace http
