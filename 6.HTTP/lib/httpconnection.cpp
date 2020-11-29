#include "httperr.h"
#include "httpconnection.h"

namespace http {

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

bool HttpConnection::is_keep_alive() const {
    return keep_alive;
}

}  // namespace http
