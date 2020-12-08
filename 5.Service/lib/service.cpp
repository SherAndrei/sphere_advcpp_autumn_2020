#include <algorithm>
#include "globallogger.h"
#include "service.h"
#include "bufconnection.h"
#include "neterr.h"

namespace {

net::BufferedConnection* get(tcp::IConnectable* p_conn) {
    return dynamic_cast<net::BufferedConnection*>(p_conn);
}

}  // namespace

namespace net {

Service::Service(const tcp::Address& addr, IServiceListener* listener)
    : IService(addr, listener) {
    server_.set_reuseaddr();
    server_.set_nonblock();
    epoll_.add(server_.socket(), net::OPTION::READ);
}

void Service::setListener(IServiceListener* listener) {
    listener_ = listener;
}

IServiceListener* Service::getListener() {
    return dynamic_cast<IServiceListener*>(listener_);
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
    IServiceListener* listener = getListener();
    if (listener == nullptr)
        throw ListenerError("Listener was set incorrectly");
    while (true) {
        log::info(std::to_string(clients_.size()) + " active connections");
        log::debug("Server waits");
        std::vector<::epoll_event> epoll_events = epoll_.wait();
        for (::epoll_event& event : epoll_events) {
            if (event.data.fd == server_.socket().fd()) {
                ConnectionUPtr conn(std::make_unique<BufferedConnection>(server_.accept_non_block()));
                clients_.push_back(IClient{std::move(conn)});
                clients_.back().iter = std::prev(clients_.end());
                BufferedConnection* p_conn = get(clients_.back().conn.get());
                log::info("Server accepted " + p_conn->address().str());
                epoll_.add(&clients_.back(), OPTION::CLOSE);

                listener->onNewConnection(*p_conn);
                if (p_conn->socket().valid()) {
                    epoll_.mod(&clients_.back(), p_conn->epoll_option_);
                }
            } else {
                IClient* p_client = static_cast<IClient*>(event.data.ptr);
                auto    it_client = p_client->iter;
                BufferedConnection* p_conn = get(p_client->conn.get());
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
                    clients_.erase(it_client);
                }
                if (p_conn->socket().valid()) {
                    epoll_.mod(p_client, p_conn->epoll_option_);
                }
            }
        }
    }
}

void Service::close() {
    server_.close();
}

}  // namespace net
