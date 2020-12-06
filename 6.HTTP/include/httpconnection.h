#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include "bufconnection.h"
#include "message.h"
#include "iTimed.h"

namespace http {

class HttpConnection : public net::BufferedConnection, protected ITimed {
 public:
    explicit HttpConnection(tcp::NonBlockConnection&& other);

 public:
    void write(const Responce& resp);
    Request request()  const;

    bool is_keep_alive() const;

    void close() override;

 protected:
    friend class HttpService;

    void subscribe(net::OPTION opt) override;
    void unsubscribe(net::OPTION opt) override;

 protected:
    Request req_;
    bool keep_alive_ = false;
};

}  // namespace http

#endif  // HTTP_CONNECTION_H
