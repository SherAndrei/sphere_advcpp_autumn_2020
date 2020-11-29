#include <algorithm>
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
        x                              \
    } catch (tcp::TimeOutError& ex) {  \
        break;                         \
    }                                  \
}

namespace {

bool is_keep_alive(const http::Request& req) {
    auto it = std::find_if(req.headers().begin(), req.headers().end(),
                          [](const http::Header& h) {
                              return h.name == "Connection";
                          });
    if (it == req.headers().end()) {
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

void HttpService::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_reuseaddr();
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
        tcp::Connection new_c = server_.accept();
        log::info("Server accepts: " + new_c.address().str());
        new_c.set_nonblock();
        mutex_.lock_shared();
        bool closed_non_empty = !closed_.empty();
        mutex_.unlock_shared();
        if (closed_non_empty) {
            mutex_.lock_shared();
            p_client = closed_.front();
            mutex_.unlock_shared();
            *p_client = HttpConnection(std::move(new_c));

            mutex_.lock();
            closed_.pop();
            mutex_.unlock();
        } else {
            manager_.emplace_back(std::move(new_c));
            p_client = &(manager_.back());
        }
        p_client->epoll_option_ = net::OPTION::READ + net::OPTION::ET_ONESHOT;
        connection_epoll_.add(p_client, net::OPTION::READ + net::OPTION::ET_ONESHOT);
        mutex_.lock_shared();
        log::info("Active connections: " + std::to_string(manager_.size() - closed_.size()));
        mutex_.unlock_shared();
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
                TRY_UNTIL_EAGAIN(
                    p_client->read_to_buffer();
                )
                try {
                    p_client->req_.parse(p_client->read_buf());
                    p_client->keep_alive = is_keep_alive(p_client->req_);
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
                    close_connection(p_client);
                    continue;
                }

                listener_->OnRequest(*p_client);

                if (!p_client->keep_alive)
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
                TRY_UNTIL_EAGAIN(
                    if (p_client->write_from_buffer() == 0u)
                        break;
                )
                if (p_client->write_buf().empty())
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

void HttpService::close_connection(HttpConnection* cn) {
    cn->close();

    mutex_.lock();
    closed_.push(cn);
    mutex_.unlock();
}

}  // namespace http
