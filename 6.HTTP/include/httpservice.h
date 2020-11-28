#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <queue>
#include <list>
#include "server.h"
#include "httplistener.h"
#include "worker.h"
#include "httpmanager.h"
#include "httpservice.h"
#include "epoll.h"

namespace http {

class HttpService {
 public:
    explicit HttpService(IHttpListener* listener, size_t workersSize);
    void setListener(IHttpListener* listener);
    void open(const tcp::Address& addr);
    void run();
    void close();

 private:
    friend class Worker;
    void subscribe(HttpConnection& cn, net::OPTION opt)   const;
    void unsubscribe(HttpConnection& cn, net::OPTION opt) const;

 private:
    IHttpListener* listener_{nullptr};
    std::vector<Worker> workers_;
    tcp::Server server_;
    HttpManager manager_;
    std::queue<HttpConnection*, std::list<HttpConnection*>> closed_;
    net::EPoll server_epoll_;
    net::EPoll connection_epoll_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
