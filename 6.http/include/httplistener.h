#ifndef HTTP_LISTENER_H
#define HTTP_LISTENER_H
#include "iListener.h"
#include "httpconnection.h"

namespace http {

class IHttpListener : public net::IListener {
 public:
    virtual void OnRequest(HttpConnection&) = 0;
};

}  // namespace http

#endif  // HTTP_LISTENER_H
