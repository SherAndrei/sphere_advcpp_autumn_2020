#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include <string>
#include "epoll.h"
#include "bufconnection.h"
#include "message.h"

namespace http {

class Worker;

class HttpConnection : public net::BufferedConnection {
 public:
    using BufferedConnection::BufferedConnection;

 public:
    void write(const Responce& resp);
    Request request()  const;

 public:
    bool is_keep_alive() const;

 private:
    friend class HttpService;
    friend class Worker;
 private:
    Request req_;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
