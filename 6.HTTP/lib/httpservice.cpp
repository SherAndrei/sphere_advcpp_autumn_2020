#include "globallogger.h"
#include "httpservice.h"
#include "option.h"
#include "tcperr.h"
#include "neterr.h"
#include "httperr.h"
#include "connection.h"

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
        workers_.emplace_back(this, i + 1);
    }

    for (auto& worker : workers_) {
        worker.set_thread(std::thread(&Worker::work, std::ref(worker)));
        log::debug("Thread " + worker.info() + " up and running");
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

    for (auto& worker : workers_) {
        worker.join();
        log::debug("Thread " + worker.info() + " finished");
    }

    log::info("Server finished");
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
