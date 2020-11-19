#include <algorithm>
#include <iostream>
#include "service.h"

namespace net {

Service::Service(IServiceListener* listener)
    : listener_(listener) {}

void Service::setListener(IServiceListener* listener) {
    listener_ = listener;
}

void Service::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.set_nonblock();
    t_serv.set_reuseaddr();
    epoll_.add(t_serv.fd(), OPTION::READ);
    server_ = std::move(t_serv);
}

static auto find_client_it(std::list<BufferedConnection>& list, int fd) {
    auto it_client = std::find_if(list.begin(), list.end(),
                       [fd](BufferedConnection& client) {
                           return client.fd().fd() == fd;
                       });
    return it_client;
}

void Service::run() {
    while (true) {
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.fd().fd()) {
                connections_.emplace_back(server_.accept(), &epoll_);
                epoll_.add(connections_.back().fd(), OPTION::UNKNOW);
                listener_->onNewConnection(connections_.back());
            } else {
                auto it_client = find_client_it(connections_, event.data.fd);
                BufferedConnection& client = *it_client;
                if (event.events & EPOLLERR) {
                    listener_->onError(client);
                } else if (event.events & EPOLLIN) {
                    size_t size;
                    size = client.read_to_buffer();
                    if (size == 0)
                        listener_->onError(client);
                    else
                        listener_->onReadAvailable(client);
                } else if (event.events & EPOLLOUT) {
                    if (!client.write_buf().empty()) {
                        size_t size = client.write_from_buffer();
                        if (size == 0)
                            listener_->onError(client);
                    } else {
                        listener_->onWriteDone(client);
                    }
                }
                if (client.epoll_option_ == OPTION::UNKNOW ||
                    event.events & EPOLLRDHUP) {
                    listener_->onClose(client);
                    client.close();
                    connections_.erase(it_client);
                }
            }
        }
    }
}

void Service::close() {
    server_.close();
}

}  // namespace net
