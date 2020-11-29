#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include <string>
#include "epoll.h"
#include "bufconnection.h"
#include "message.h"

namespace http {

class HttpConnection : public net::BufferedConnection {
 public:
    using BufferedConnection::BufferedConnection;

 public:
    void write(const Responce& resp);
    Request request()  const;

    void subscribe(net::OPTION opt) override;
    void unsubscribe(net::OPTION opt) override;

 public:
    bool is_keep_alive() const;

 private:
    friend class HttpService;
 private:
    Request req_;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
