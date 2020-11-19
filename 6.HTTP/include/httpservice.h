#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <list>
#include "service.h"
#include "epoll.h"
#include "httplistener.h"
#include "httpconnection.h"

namespace http {

class HttpService : public net::Service {
 public:
    explicit HttpService(IHttpListener* listener);
    void setListener(IHttpListener* listener);

 private:
    IHttpListener* listener_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
