#include "httpconnection.h"
#include "httperr.h"

namespace http {

HttpConnection::HttpConnection(tcp::NonBlockConnection&& other)
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
    tcp::NonBlockConnection::close();
}

void HttpConnection::reset_time_of_last_activity() {
    start_ = std::chrono::system_clock::now();
}

bool HttpConnection::is_keep_alive() const {
    return keep_alive_;
}

bool HttpConnection::is_timed_out(size_t timeo) const {
    time_point_t now = std::chrono::system_clock::now();
    auto limit = std::chrono::seconds(timeo);
    return (now - start_) > limit;
}

}  // namespace http
