#include "httpconnection.h"
#include "httperr.h"

namespace http {

HttpConnection::HttpConnection(tcp::NonBlockConnection&& other)
    : net::BufferedConnection(std::move(other)) {}

void HttpConnection::write(const Responce& res) {
    write_ = res.str();
}

Request  HttpConnection::request()  const {
    return req_;
}

void HttpConnection::subscribe(net::OPTION opt) {
    epoll_option_ = epoll_option_ + opt + net::OPTION::ET_ONESHOT;
}

void HttpConnection::unsubscribe(net::OPTION opt) {
    epoll_option_ = epoll_option_ - opt + net::OPTION::ET_ONESHOT;
}

void HttpConnection::close() {
    epoll_option_ = net::OPTION::ET_ONESHOT;
    tcp::NonBlockConnection::close();
}

bool HttpConnection::is_keep_alive() const {
    return keep_alive_;
}


}  // namespace http
