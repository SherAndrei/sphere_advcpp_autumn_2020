#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include <string>
#include "epoll.h"
#include "timeout.h"
#include "bufconnection.h"
#include "message.h"

namespace http {

class HttpConnection : public net::BufferedConnection {
 public:
    using BufferedConnection::BufferedConnection;
    explicit HttpConnection(tcp::Connection && other);

    HttpConnection(HttpConnection && other)             = default;
    HttpConnection& operator= (HttpConnection && other) = default;

    virtual ~HttpConnection() = default;

 public:
    void write(const Responce& resp);
    Request request()  const;

    bool is_keep_alive() const;
    bool is_timed_out(size_t timeo) const;

    void close() override;

 protected:
    friend class HttpService;
    void reset_time_of_last_activity();
    void subscribe(net::OPTION opt) override;
    void unsubscribe(net::OPTION opt) override;

 protected:
    time_point_t start_;
    Request req_;
    bool keep_alive_ = false;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
