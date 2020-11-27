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

 private:
    friend class Worker;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
