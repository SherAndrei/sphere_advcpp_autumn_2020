#include "globallogger.h"
#include "httpservice.h"
#include "option.h"
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
    t_serv.set_reuseaddr();
    // При  наступлении события, ядро отключает события для дескриптора
    // (Читаем в буфер?)Поток должен получить EAGAIN и подписаться снова
    // Управление получает только один поток
    // epoll_.add(t_serv.fd(), net::OPTION::READ
    //                       + net::OPTION::EDGETRIGGERED
    //                       + net::OPTION::ONESHOT);
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

    while (true) {
        log::debug("Server waits");
        tcp::Connection new_c = server_.accept();
        log::debug("Server accepts: " + new_c.address().str());
        new_c.set_nonblock();
        manager_.emplace_back(std::move(new_c));
        connection_epoll_.add(&(manager_.back()), net::OPTION::UNKNOW);
        subscribe(manager_.back(), net::OPTION::READ);
        // // TODO: LOCK
        // while (!manager_.front().fd().valid()) {
        //     manager_.pop();
        // }
        log::info("Active connections: " + std::to_string(manager_.size()));
    }

    for (auto& worker : workers_) {
        worker.join();
        log::debug("Thread " + worker.info() + " finished");
    }

    log::info("Server finished");
}

void HttpService::subscribe(HttpConnection& cn, net::OPTION opt)   const {
    cn.epoll_option_ = cn.epoll_option_ + opt
                     + net::OPTION::EDGETRIGGERED
                     + net::OPTION::ONESHOT;
    connection_epoll_.mod(&(cn), cn.epoll_option_);
}
void HttpService::unsubscribe(HttpConnection& cn, net::OPTION opt) const {
    cn.epoll_option_ = cn.epoll_option_ - opt
                     + net::OPTION::EDGETRIGGERED
                     + net::OPTION::ONESHOT;
    connection_epoll_.mod(&(cn), cn.epoll_option_);
}



}  // namespace http
