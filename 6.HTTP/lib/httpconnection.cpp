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
