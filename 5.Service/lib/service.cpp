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
    t_serv.set_reuseaddr();
    epoll_.add(t_serv.fd());
    server_ = std::move(t_serv);
}


void net::Service::run() {
    std::vector<::epoll_event> epoll_events;
    while (true) {
        epoll_events = epoll_.wait();
        for (auto it = epoll_events.begin(); it != epoll_events.end(); it++) {
            if (it->data.fd == server_.fd().fd()) {
                connections_.emplace_back(server_.accept());
                epoll_.add(connections_.back().fd());
                listener_->onNewConnection(connections_.back());
            } else if (it->events & EPOLLRDHUP) {
                int closed = it->data.fd;
                auto client = std::find_if(connections_.begin(), connections_.end(),
                                   [closed](net::BufferedConnection& client) {
                                       return client.fd().fd() == closed;
                                   });
                listener_->onClose(*client);
                connections_.erase(client);
            } else if (it->events & EPOLLIN) {
                int to_read = it->data.fd;
                auto it_client = std::find_if(connections_.begin(), connections_.end(),
                                   [to_read](net::BufferedConnection& client) {
                                       return client.fd().fd() == to_read;
                                   });
                auto& client = *it_client;
                client.read_to_buf();
                listener_->onReadAvailable(&(client));
            } else if (it->events & EPOLLOUT) {
                // listener_->onWriteDone(*client);
            } else if (it->events & EPOLLERR) {
                // listener_->onError(*client);
            }
        }
    }
}



void net::Service::close() {
    server_.close();
}
