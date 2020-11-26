#ifndef HTTP_WORKER_H
#define HTTP_WORKER_H
#include <thread>
#include "epoll.h"

namespace http {

class HttpService;

class Worker {
 public:
    explicit Worker(HttpService* service);
    ~Worker() = default;

    Worker(const Worker& other)            = delete;
    Worker& operator=(const Worker& other) = delete;

    Worker(Worker&& other)            = default;
    Worker& operator=(Worker&& other) = default;
 public:
    void set_thread(std::thread&& other);

 public:
    void work();

 private:
    HttpService  *service_;
    std::thread   thread_;
    net::EPoll    epoll_;
};

}  // namespace http

#endif  // HTTP_WORKER_H

