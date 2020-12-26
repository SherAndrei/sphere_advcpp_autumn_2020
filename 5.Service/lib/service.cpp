#include <algorithm>
#include "globallogger.h"
#include "service.h"
#include "bufconnection.h"
#include "neterr.h"

namespace {

net::BufferedConnection* get(tcp::BaseConnection* p_conn) {
    return dynamic_cast<net::BufferedConnection*>(p_conn);
}

}  // namespace

namespace net {

Service::Service(const tcp::Address& addr, IServiceListener* listener)
    : BaseService(addr, listener) {
    server_.socket().set_nonblock();
    server_.socket().set_reuseaddr();
    epoll_.add(server_.socket(), OPTION::READ);
}

void Service::setListener(IServiceListener* listener) {
    listener_ = listener;
}

IServiceListener* Service::getListener() {
    return dynamic_cast<IServiceListener*>(listener_);
}

void Service::open(const tcp::Address& addr) {
    tcp::Server t_serv(addr);
    t_serv.socket().set_nonblock();
    t_serv.socket().set_reuseaddr();
    epoll_.add(t_serv.socket(), OPTION::READ);
    server_ = std::move(t_serv);
    log::info("Server " + server_.address().str() + " up and running");
}

void Service::run() {
    IServiceListener* listener = getListener();
    if (listener == nullptr)
        throw ListenerError("Listener was set incorrectly");
    while (true) {
        log::info(std::to_string(connections_.size()) + " active connections");
        log::debug("Server waits");
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.socket().fd()) {
                connections_.emplace_back(std::make_unique<BufferedConnection>(server_.accept_non_block()));
                BufferedConnection* p_conn = get(connections_.back().u_conn.get());
                log::info("Server accepted " + p_conn->address().str());
                epoll_.add(p_conn->socket(), OPTION::CLOSE);

                listener->onNewConnection(*p_conn);
                if (p_conn->socket().valid()) {
                    epoll_.mod(p_conn->socket(), p_conn->epoll_option_);
                }
            } else {
                auto it_client = std::find_if(connections_.begin(), connections_.end(),
                                              [&] (const ConnectionAndData& cn_and_data) {
                                                  return cn_and_data.u_conn->socket().fd() == event.data.fd;
                                              });
                BufferedConnection* p_conn = get((*it_client).u_conn.get());
                if (event.events & EPOLLERR) {
                    log::error("Server encountered EPOLLERR from " + p_conn->address().str());
                    listener->onError(*p_conn);
                } else if (event.events & EPOLLIN) {
                    log::debug("Server encountered EPOLLIN from " + p_conn->address().str());
                    size_t size;
                    size = p_conn->read_to_buffer();
                    if (size == 0)
                        listener->onError(*p_conn);
                    else
                        listener->onReadAvailable(*p_conn);
                } else if (event.events & EPOLLOUT) {
                    log::debug("Server encountered EPOLLOUT from " + p_conn->address().str());
                    if (!p_conn->write_buf().empty()) {
                        size_t size = p_conn->write_from_buffer();
                        if (size == 0)
                            listener->onError(*p_conn);
                    } else {
                        listener->onWriteDone(*p_conn);
                    }
                }
                if (p_conn->epoll_option_ == OPTION::UNKNOWN ||
                    event.events & EPOLLRDHUP) {
                    listener->onClose(*p_conn);
                    p_conn->close();
                    log::info("Server closed " + p_conn->address().str());
                    connections_.erase(it_client);
                }
                if (p_conn->socket().valid()) {
                    epoll_.mod(p_conn->socket(), p_conn->epoll_option_);
                }
            }
        }
    }
}

void Service::close() {
    server_.close();
}

}  // namespace net
