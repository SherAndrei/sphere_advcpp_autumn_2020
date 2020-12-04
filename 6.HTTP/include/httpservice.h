#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <list>
#include "timeout.h"
#include "server.h"
#include "httplistener.h"
#include "httpmanager.h"
#include "httpservice.h"
#include "epoll.h"

namespace http {

class HttpService {
 public:
    explicit HttpService(IHttpListener* listener, size_t workersSize);
    ~HttpService() = default;

    HttpService(HttpService& other)             = delete;
    HttpService& operator=(HttpService&  other) = delete;

    HttpService(HttpService&& other)            = default;
    HttpService& operator=(HttpService&& other) = default;

 public:
    void setListener(IHttpListener* listener);
    size_t connections_size();

 public:
    void open(const tcp::Address& addr);
    void run();
    void close();

 private:
    void work(size_t thread_num);
    void subscribe(HttpConnection& cn, net::OPTION opt)   const;
    void unsubscribe(HttpConnection& cn, net::OPTION opt) const;

    bool try_replace_closed_with_new_connection(HttpConnection*& p_client, tcp::Connection&& cn);
    bool try_read_request(HttpConnection* p_client, size_t thread_num);
    bool try_write_responce(HttpConnection* p_client);
    bool try_reset_last_activity_time(HttpConnection* p_client);

    void close_connection(HttpConnection* cn);
    void close_if_timed_out(HttpConnection* cn);

 private:
    IHttpListener* listener_{nullptr};
    std::vector<std::thread> workers_;
    tcp::Server server_;
    HttpManager manager_;
    std::queue<HttpConnection*, std::list<HttpConnection*>> closed_;
    net::EPoll connection_epoll_;
    std::mutex mutex_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
