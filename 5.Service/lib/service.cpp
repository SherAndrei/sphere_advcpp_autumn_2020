#include "service.h"


net::Service::Service(std::shared_ptr<IServiceListener> listener)
    : listener_(listener)
    , connections_()
    , server_()
    , epoll_()
    {}

void net::Service::setListener(std::shared_ptr<IServiceListener> listener) {
    listener_ = listener;
}

void net::Service::open(const tcp::Address& addr) {
    tcp::Server t_serv{addr,
                       tcp::Socket{AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0}};
    ::epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = t_serv.fd();
    epoll_.add(t_serv.fd(), &event);
    server_ = std::move(t_serv);
}


void net::Service::run() {
    int epoll_events;
    while (true) {
        epoll_events = epoll_.wait();
    }
}

