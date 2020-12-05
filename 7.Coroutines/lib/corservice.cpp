#include "globallogger.h"
#include "neterr.h"
#include "httperr.h"
#include "tcperr.h"
#include "message.h"
#include "coroutine.h"
#include "corservice.h"

namespace {

namespace {

http::HttpConnection* get(tcp::IConnectable* p_conn) {
    return dynamic_cast<http::HttpConnection*>(p_conn);
}

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
            cor::yield();
            continue;
        } catch (IncorrectData& ind) {
            log::warn("Worker " + std::to_string(th_num) + " got incorrect request from "
                                                         + p_conn->address().str());
            close_client(p_client);
            return false;
        }
    }
    return true;
}

bool CoroutineService::try_write_responce(net::IClient* p_client) {
    HttpConnection* p_conn = get(p_client->conn.get());
    while (true) {
        try {
            p_conn->write_from_buffer();
        } catch (tcp::TimeOutError& ex) {
            if (!p_conn->write_buf().empty()) {
                cor::yield();
                continue;
            }
            break;
        } catch (tcp::DescriptorError& ex) {
            log::warn(std::string(ex.what()) + " at " + p_conn->address().str());
            close_client(p_client);
            return false;
        }
    }
    return true;
}

}  // namespace cor
}  // namespace http
