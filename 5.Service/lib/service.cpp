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

static auto find_client_it(std::list<BufferedConnection>* list, int fd) {
    auto it_client = std::find_if(list->begin(), list->end(),
                       [fd](BufferedConnection& client) {
                           return client.fd().fd() == fd;
                       });
    return it_client;
}

void Service::run() {
    ssize_t read_size;
    bool we_know_size = false;
    while (true) {
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.fd().fd()) {
                connections_.emplace_back(server_.accept(), &epoll_);
                epoll_.add(connections_.back().fd(), OPTION::UNKNOW);
                listener_->onNewConnection(&(connections_.back()));
            } else  {
                auto it_client = find_client_it(&connections_, event.data.fd);
                BufferedConnection& client = *it_client;
                if (event.events & EPOLLRDHUP) {
                    listener_->onClose(&client);
                    client.close();
                    connections_.erase(it_client);
                } else if (event.events & EPOLLERR) {
                    listener_->onError(&client);
                } else if (event.events & EPOLLIN) {
                    // FIXME : multiple clients share 'we know size' flag
                    // FIXME : if client send size bigger than actual delivered size ?
                    if (!we_know_size &&
                        (client.connection_.read(&read_size, sizeof(size_t)) != sizeof(size_t))) {
                            listener_->onError(&client);
                            continue;
                    }
                    we_know_size = true;
                    read_size -= client.connection_.read(client.read_.remaining_space(), read_size);
                    if (read_size == 0u) {
                        listener_->onReadAvailable(&client);
                        we_know_size = false;
                        client.read_.clear();
                    } else if (read_size < 0) {
                        listener_->onError(&client);
                    }
                } else if (event.events & EPOLLOUT) {
                    // FIXME : no work with buffer
                    auto& wbuf = client.write_;
                    if (!client.write_.empty()) {
                        if (client.connection_.write(wbuf.data(), wbuf.size()) != wbuf.size()) {
                            listener_->onError(&client);
                        }
                        listener_->onWriteDone(&client);
                    }
                }
            }
        }
    }
}

void Service::close() {
    server_.close();
}

}  // namespace net
