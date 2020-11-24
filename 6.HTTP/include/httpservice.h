#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include "server.h"
#include "address.h"
#include "worker.h"
#include "connection_manager.h"
#include "epoll.h"
#include "httplistener.h"
#include "httpconnection.h"

namespace http {

class HttpService {
 public:
    explicit HttpService(IHttpListener* listener);
    void setListener(IHttpListener* listener);
    void setWorkersSize(size_t size);
    void open(const tcp::Address& addr);
    void run();

 private:
    IHttpListener* listener_{nullptr};
    std::vector<Worker> workers_;
    tcp::Server server_;
    net::ConnectionManager manager_;
    net::EPoll epoll_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
