#include <algorithm>
#include "globallogger.h"
#include "service.h"
#include "neterr.h"

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
    epoll_.add(t_serv.socket(), OPTION::READ);
    server_ = std::move(t_serv);
    log::info("Server " + server_.address().str() + " up and running");
}

void Service::run() {
    if (listener_ == nullptr)
        throw ListenerError("Listener was not set");
    while (true) {
        log::info(std::to_string(manager_.size()) + " active connections");
        log::debug("Server waits");
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.socket().fd()) {
                manager_.emplace_back(server_.accept());
                BufferedConnection& client = manager_.back();
                log::info("Server accepted " + client.address().str());
                epoll_.add(client.socket(), OPTION::UNKNOW);
                listener_->onNewConnection(client);
                if (client.socket().valid()) {
                    epoll_.mod(client.socket(), client.epoll_option_);
                }
            } else {
                auto it_client = std::find_if(manager_.begin(), manager_.end(),
                                             [&] (const BufferedConnection& bc) {
                                                return bc.socket().fd() == event.data.fd;
                                             });
                BufferedConnection& client = *(it_client);
                if (event.events & EPOLLERR) {
                    log::error("Server encountered EPOLLERR from " + client.address().str());
                    listener_->onError(client);
                } else if (event.events & EPOLLIN) {
                    log::debug("Server encountered EPOLLIN from " + client.address().str());
                    size_t size;
                    size = client.read_to_buffer();
                    if (size == 0)
                        listener_->onError(client);
                    else
                        listener_->onReadAvailable(client);
                } else if (event.events & EPOLLOUT) {
                    log::debug("Server encountered EPOLLOUT from " + client.address().str());
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
                    log::info("Server closed " + client.address().str());
                    manager_.erase(it_client);
                }
                if (client.socket().valid()) {
                    epoll_.mod(client.socket(), client.epoll_option_);
                }
            }
        }
    }
}

void Service::close() {
    server_.close();
}

}  // namespace net
