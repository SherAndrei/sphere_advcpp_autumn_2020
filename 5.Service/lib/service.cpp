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
    ::epoll_event event{};
    event.events = EPOLLIN | EPOLLRDHUP;
    event.data.fd = fd;

}


void net::Service::run() {
    while (true) {
        // cc
    }
}

