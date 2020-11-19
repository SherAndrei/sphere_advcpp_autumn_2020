#ifndef HTTP_LISTENER_H
#define HTTP_LISTENER_H
#include "httpservice.h"
#include "httpconnection.h"

namespace http {

class IHttpListener {
    virtual void OnRequest(HttpService&, HttpConnection&) = 0;
};

}  // namespace http

#endif  // HTTP_LISTENER_H
