#include "globallogger.h"
#include "httpservice.h"
#include "option.h"
#include "tcperr.h"
#include "neterr.h"
#include "httperr.h"
#include "connection.h"

#define TRY_UNTIL_EAGAIN(x)            \
for(;;) {                              \
    try {                              \
        if (x() == 0)                  \
            break;                     \
    } catch (tcp::TimeOutError& ex) {  \
        break;                         \
    }                                  \
}

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

void HttpService::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_nonblock();
    t_serv.set_reuseaddr();
    server_epoll_.add(t_serv.socket(), net::OPTION::READ + net::OPTION::ET_ONESHOT);
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
    while (true) {
        log::debug("Server waits");
        std::vector<::epoll_event> events = server_epoll_.wait();
        log::debug("Server got " + std::to_string(events.size()) + " new events");
        for (::epoll_event event : events) {
            if (event.data.fd == server_.socket().fd()) {
                for (;;) {
                    try {
                        tcp::Connection new_c = server_.accept();
                        log::info("Server accepts: " + new_c.address().str());
                        new_c.set_nonblock();
                        if (!closed_.empty()) {
                            p_client = closed_.front();
                            *p_client = HttpConnection(std::move(new_c));
                            closed_.pop();
                        } else {
                            manager_.emplace_back(std::move(new_c));
                            p_client = &(manager_.back());
                        }
                        connection_epoll_.add(p_client, net::OPTION::READ + net::OPTION::ET_ONESHOT);
                        server_epoll_.add(p_client, net::OPTION::CLOSE + net::OPTION::ET_ONESHOT);
                    } catch (tcp::TimeOutError& ex) {
                        break;
                    }
                }
            } else if (event.events & EPOLLRDHUP) {
                p_client = static_cast<HttpConnection*>(event.data.ptr);
                p_client->close();
                closed_.push(p_client);
                log::info("Server closes " + p_client->address().str());
            }
        }
        server_epoll_.mod(server_.socket(), net::OPTION::READ + net::OPTION::ET_ONESHOT);
        log::info("Active connections: " + std::to_string(manager_.size() - closed_.size()));
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

                if (event.events & EPOLLIN) {
                log::debug("Worker " + std::to_string(th_num)
                                     + " encounters EPOLLIN from " + p_client->address().str());
                TRY_UNTIL_EAGAIN(p_client->read_to_buffer);
                try {
                    p_client->req_.parse(p_client->read_buf());
                } catch (ExpectingData& exd) {
                    log::warn("Worker " + std::to_string(th_num)
                                        + " got incomplete request from "
                                        + p_client->address().str());
                    subscribe(*p_client, net::OPTION::READ);
                    continue;
                } catch (IncorrectData& ind) {
                    log::warn("Worker " + std::to_string(th_num)
                                        + " got incorrect request from "
                                        + p_client->address().str());
                    // TODO: p_client->close();
                    continue;
                }
                // TODO: LOCK
                listener_->OnRequest(*p_client);
                if (!p_client->is_keep_alive())
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
                TRY_UNTIL_EAGAIN(p_client->write_from_buffer);
                if (p_client->write_buf().empty())
                    p_client->unsubscribe(net::OPTION::WRITE);
                log::info("Worker " + std::to_string(th_num)
                                    + " successfully wrote to "
                                    + p_client->address().str());
            } else {
                log::error("Worker " + std::to_string(th_num) + " encounters "
                          + (event.events & EPOLLRDHUP ? "EPOLLRDHUP" : "EPOLLERR")
                          + " from " + p_client->address().str());
                // TODO: p_client->close();
                continue;
            }
            subscribe(*p_client, p_client->epoll_option_);
        }
    }
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

}  // namespace http
