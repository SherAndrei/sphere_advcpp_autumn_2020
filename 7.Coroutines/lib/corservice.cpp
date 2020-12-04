#include "globallogger.h"
#include "neterr.h"
#include "httperr.h"
#include "tcperr.h"
#include "message.h"
#include "coroutine.h"
#include "corservice.h"

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
namespace cor {

CoroutineService::CoroutineService(ICoroutineListener* listener, size_t workerSize) {
    workers_.reserve(std::min(static_cast<size_t>(std::thread::hardware_concurrency()),
                        workerSize));
    if (workers_.capacity() == 0u) {
        throw WorkerError("Workers size == 0");
    }
    setListener(listener);
}

void CoroutineService::setListener(ICoroutineListener* listener) {
    listener_ = listener;
}

size_t CoroutineService::connections_size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return manager_.size() - closed_.size();
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
            new_c = CorConnection(server_.accept_non_block());
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
        p_client->set_routine(cor::create([this, p_client] { this->serve_client(p_client); }));
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

void CoroutineService::serve_client(CorConnection* p_client) {
    if (!try_reset_last_activity_time(p_client))
        return;

    log::debug("Worker encounters EPOLLIN from "
                      + p_client->address().str());
    if (!try_read_request(p_client)) {
        return;
    }
    p_client->write(listener_->OnRequest(std::move(p_client->req_)));
    p_client->read_.clear();
    if (!p_client->keep_alive_) {
        unsubscribe(*p_client, net::OPTION::READ);
    }
    subscribe(*p_client, net::OPTION::WRITE);
    log::info("Worker successfully read from "
                     + p_client->address().str());
    cor::yield();

    log::debug("Worker encounters EPOLLOUT from "
                      + p_client->address().str());
    if (try_write_responce(p_client))
        p_client->unsubscribe(net::OPTION::WRITE);
    log::info("Worker successfully wrote to "
                        + p_client->address().str());

    if (p_client->epoll_option_ == net::OPTION::ET_ONESHOT) {
        close_connection(p_client);
    } else {
        subscribe(*p_client, net::OPTION::UNKNOWN);
    }
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
            cor::resume(p_client->routine_id());
        }
    }
}

CorConnection* CoroutineService::try_replace_closed_with_new_connection(CorConnection&& cn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!closed_.empty()) {
        CorConnection* p_cn = closed_.front();
        *p_cn = std::move(cn);
        closed_.pop();
        return p_cn;
    }
    return nullptr;
}

bool CoroutineService::try_read_request(CorConnection* p_client) {
    while (true) {
        try {
            if (p_client->read_to_buffer() == 0) {
                log::info("Client " + p_client->address().str() + " closed unexpectedly");
                close_connection(p_client);
                return false;
            }
            try {
                Request req(p_client->read_buf());
                p_client->keep_alive_ = is_keep_alive(req.headers());
                p_client->req_ = std::move(req);
            } catch (ExpectingData& exd) {
                log::warn("Worker got incomplete request from "
                                    + p_client->address().str());
                subscribe(*p_client, net::OPTION::READ);
                cor::yield();
                continue;
            } catch (IncorrectData& ind) {
                log::warn("Worker got incorrect request from "
                                 + p_client->address().str());
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

    return true;
}

bool CoroutineService::try_write_responce(CorConnection* p_client) {
    while (true) {
        try {
            p_client->write_from_buffer();
            if (!p_client->write_buf().empty()) {
                cor::yield();
                continue;
            } else {
                break;
            }
        } catch (tcp::TimeOutError& ex) {
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_client->address().str());
            close_connection(p_client);
            return false;
        }
    }
    return true;
}

void CoroutineService::close_if_timed_out(CorConnection* cn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (cn->socket().valid() && cn->is_timed_out()) {
        log::info("Connection timed out: " + cn->address().str());
        closed_.push(cn);
        cn->close();
    }
}

void CoroutineService::close_connection(CorConnection* cn) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (cn->socket().valid()) {
        log::info("Closing connection: " + cn->address().str());
        closed_.push(cn);
        cn->close();
    }
}


}  // namespace cor
}  // namespace http
