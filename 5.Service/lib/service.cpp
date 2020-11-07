#include <algorithm>
#include "service.h"

net::Service::Service(IServiceListener* listener)
    : listener_(listener)
    , connections_()
    , server_()
    , epoll_()
    {}

void net::Service::setListener(IServiceListener* listener) {
    listener_ = listener;
}

void net::Service::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_nonblock();

    epoll_.add(t_serv.fd());
    server_ = std::move(t_serv);
}


void net::Service::run() {
    std::vector<::epoll_event> epoll_events;
    while (true) {
        epoll_events = epoll_.wait();
        for (const auto& event : epoll_events) {
            if (event.data.fd == server_.fd().fd()) {
                connections_.emplace_back(server_.accept());
                epoll_.add(connections_.back().fd());
                listener_->onNewConnection(connections_.back());
            } else if (event.events & EPOLLRDHUP) {
                int closed = event.data.fd;
                auto client = std::find_if(connections_.begin(), connections_.end(),
                                   [closed](net::BufferedConnection& client) {
                                       return client.fd().fd() == closed;
                                   });
                listener_->onClose(*client);
                connections_.erase(client);
            }
        }
    }
}

