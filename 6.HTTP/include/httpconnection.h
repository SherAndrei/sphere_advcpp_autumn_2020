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

 public:
    void write(const Responce& resp);
    Request request()  const;

    bool is_keep_alive() const;
    bool is_timed_out() const;

 private:
    friend class HttpService;
    void reset_time_of_last_activity();
    void subscribe(net::OPTION opt) override;
    void unsubscribe(net::OPTION opt) override;
    void close() override;

 private:
    time_point_t start_;
    Request req_;
    bool keep_alive_ = false;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
