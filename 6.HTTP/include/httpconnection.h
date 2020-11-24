#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include "epoll.h"
#include "bufconnection.h"
#include "message.h"

namespace http {

class HttpService;

class HttpConnection {
    HttpConnection() = default;

    HttpConnection& operator= (const HttpConnection&  other) = delete;
    HttpConnection(const HttpConnection& other)              = delete;

    HttpConnection(HttpConnection && other)             = default;
    HttpConnection& operator= (HttpConnection && other) = default;

    ~HttpConnection() = default;

 public:
    void write(const Message& mess);

    Request  request()  const;
    Responce responce() const;
    void close();

 public:
    tcp::Descriptor& fd();
    const tcp::Descriptor& fd() const;
    tcp::Address adress() const;

 private:
    friend class HttpService;
    size_t read_to_request();
    size_t write_from_reply();

 private:
    Request  req_;
    Responce resp_;
    tcp::Connection connection_;
    net::EPoll* p_epoll_;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
