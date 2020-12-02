#include "httperr.h"
#include "httpconnection.h"

namespace http {

HttpConnection::HttpConnection(tcp::Connection && other)
    : net::BufferedConnection(std::move(other))
    , start_(std::chrono::system_clock::now()) {}

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
    connection_.close();
}

void HttpConnection::reset_time_of_last_activity() {
    start_ = std::chrono::system_clock::now();
}

bool HttpConnection::is_keep_alive() const {
    return keep_alive_;
}

bool HttpConnection::is_timed_out() const {
    time_point_t now = std::chrono::system_clock::now();
    auto limit = std::chrono::seconds((keep_alive_ ? KEEP_ALIVE_CONNECTION_TIMEOUT : CONNECTION_TIMEOUT));
    return (now - start_) > limit;
}

}  // namespace http
