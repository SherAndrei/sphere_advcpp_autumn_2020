#include "httpservice.h"
#include "option.h"
#include "neterr.h"
#include "httperr.h"

namespace http {

HttpService::HttpService(IHttpListener* listener, size_t workerSize) {
    setListener(listener);
    setWorkersSize(workerSize);
}

void HttpService::setListener(IHttpListener* listener) {
    listener_ = listener;
}

void HttpService::setWorkersSize(size_t size) {
    size_t nthreads = static_cast<size_t>(std::thread::hardware_concurrency());
    for (size_t i = 0; i < std::min(size, nthreads); i++) {
        workers_.emplace_back(this);
    }
}

void HttpService::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_nonblock();
    t_serv.set_reuseaddr();
    epoll_.add(t_serv.fd(), net::OPTION::READ + net::OPTION::ONESHOT);
    server_ = std::move(t_serv);
}

void HttpService::run() {
    if (listener_ == nullptr)
        throw net::ListenerError("Listener was not set");
    if (workers_.size() == 0)
        throw WorkerError("Worker size was not set");

    for (auto& worker : workers_) {
        worker.set_thread(std::thread(&Worker::work, std::ref(worker)));
    }

    while (true) {
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.fd().fd()) {
                manager_.emplace(std::make_shared<HttpConnection>(server_.accept(), &epoll_));
                epoll_.add(manager_.last().fd(), net::OPTION::READ);
            }
        }
    }
}



}  // namespace http
