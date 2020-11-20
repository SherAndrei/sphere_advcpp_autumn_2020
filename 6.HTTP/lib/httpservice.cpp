#include "httpservice.h"
#include "option.h"
#include "neterr.h"
#include "httperr.h"
#include <iostream>

namespace http {

HttpService::HttpService(IHttpListener* listener)
    : listener_(listener) {}

void HttpService::setWorkersSize(size_t size) {
    workers_.resize(size);
}

void HttpService::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_nonblock();
    t_serv.set_reuseaddr();
    epoll_.add(t_serv.fd(), net::OPTION::READ);  // TODO: epolloneshot
    server_ = std::move(t_serv);
}

// один epoll у сервера
// onnections у сервера
// сервер делает accept
void HttpService::run() {
    if (listener_ == nullptr)
        throw net::ListenerError("Listener was not set");
    if (workers_.size() == 0)
        throw WorkerError("Worker size was not set");
    while (true) {
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.fd().fd()) {
                manager_.emplace(server_.accept(), &epoll_);
                epoll_.add(manager_.last().fd(), net::OPTION::READ);
            } else {
                auto it_client = manager_.find(event.data.fd);
                HttpConnection& client = *it_client;
                if (event.events & EPOLLERR) {
                    std::cout << "EPOLLERR happened" << std::endl;
                } else if (event.events & EPOLLIN) {
                    
                } else if (event.events & EPOLLOUT) {
                    std::cout << "EPOLLOUT happened" << std::endl;
                }
                if (event.events & EPOLLRDHUP) {
                    std::cout << "EPOLLRDHUP happened" << std::endl;
                }
            }
        }
    }
}

}  // namespace http
