#include "httperr.h"
#include "httpconnection.h"

namespace http {

void HttpConnection::write(const Responce& res) {
    write_ = res.str();
}

Request  HttpConnection::request()  const {
    return Request(read_);
}

}  // namespace http
