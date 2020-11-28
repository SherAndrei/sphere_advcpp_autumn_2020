#include <algorithm>
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
    auto it = std::find_if(req_.headers_.begin(), req_.headers_.end(),
                          [](const Header& h) {
                              return h.name == "Connection";
                          });
    if (it == req_.headers_.end()) {
        return false;
    }

    return it->value.find("keep-alive") != it->value.npos;
}

}  // namespace http
