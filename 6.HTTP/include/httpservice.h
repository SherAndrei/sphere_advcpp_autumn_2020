#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include "server.h"
#include "httplistener.h"
#include "worker.h"
#include "connection_manager.h"
#include "epoll.h"

namespace http {

class HttpService {
 public:
    explicit HttpService(IHttpListener* listener, size_t workersSize);
    void setListener(IHttpListener* listener);
    void setWorkersSize(size_t size);
    void open(const tcp::Address& addr);
    void run();
    void close();

 private:
    friend class Worker;

 private:
    IHttpListener* listener_{nullptr};
    std::vector<Worker> workers_;
    tcp::Server server_;
    net::ConnectionManager manager_;
    net::EPoll epoll_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
