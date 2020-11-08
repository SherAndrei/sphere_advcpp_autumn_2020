#include <algorithm>
#include <iostream>
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
    epoll_.set_option(OPTION::READ);
    epoll_.add(t_serv.fd());
    server_ = std::move(t_serv);
}

void net::Service::closeConnection(net::BufferedConnection* bc) {
    epoll_.del(bc->fd());
    bc->close();
}

static auto find_client_it(std::list<net::BufferedConnection>* list, int fd) {
    auto it_client = std::find_if(list->begin(), list->end(),
                       [fd](net::BufferedConnection& client) {
                           return client.fd().fd() == fd;
                       });
    return it_client;
}

void net::Service::run() {
    std::vector<::epoll_event> epoll_events;
    while (true) {
        epoll_events = epoll_.wait();
        for (auto it = epoll_events.begin(); it != epoll_events.end(); it++) {
            if (it->data.fd == server_.fd().fd()) {
                connections_.emplace_back(server_.accept(), &epoll_);
                listener_->onNewConnection(&(connections_.back()));
            } else if (it->events & EPOLLRDHUP) {
                auto it_client = find_client_it(&connections_, it->data.fd);
                auto& client = *it_client;
                listener_->onClose(&client);
                closeConnection(&client);
                connections_.erase(it_client);
            } else if (it->events & EPOLLERR) {
                auto it_client = find_client_it(&connections_, it->data.fd);
                auto& client = *it_client;
                listener_->onError(&client);
            } else if (it->events & EPOLLIN) {
                auto it_client = find_client_it(&connections_, it->data.fd);
                auto& client = *it_client;
                if (client.read_buf().empty())
                    listener_->onReadAvailable(&client);
            } else if (it->events & EPOLLOUT) {
                auto it_client = find_client_it(&connections_, it->data.fd);
                auto& client = *it_client;
                listener_->onWriteDone(&client);
            }
        }
    }
}



void net::Service::close() {
    server_.close();
}
