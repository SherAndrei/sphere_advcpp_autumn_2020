#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <thread>
#include <shared_mutex>
#include <queue>
#include <list>
#include "server.h"
#include "httplistener.h"
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
    void work(size_t thread_num);
    void subscribe(HttpConnection& cn, net::OPTION opt)   const;
    void unsubscribe(HttpConnection& cn, net::OPTION opt) const;

 private:
    IHttpListener* listener_{nullptr};
    std::vector<std::thread> workers_;
    tcp::Server server_;
    HttpManager manager_;
    std::queue<HttpConnection*, std::list<HttpConnection*>> closed_;
    net::EPoll server_epoll_;
    net::EPoll connection_epoll_;
    std::shared_mutex mutex_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
