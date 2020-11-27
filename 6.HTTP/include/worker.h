#ifndef HTTP_WORKER_H
#define HTTP_WORKER_H
#include <thread>
#include <memory>
#include <string>
#include "epoll.h"

namespace http {

class HttpService;

class Worker {
 public:
    Worker(HttpService* service, size_t id, size_t nthreads);
    ~Worker() = default;

    Worker(const Worker& other)            = delete;
    Worker& operator=(const Worker& other) = delete;

    Worker(Worker&& other)            = default;
    Worker& operator=(Worker&& other) = default;

 public:
    std::string info() const;

 public:
    void set_thread(std::thread&& other);
    void join();

 public:
    void work();

 private:
    HttpService  *service_;
    size_t id_;
    size_t nthreads_;
    std::thread   thread_;
};

}  // namespace http

#endif  // HTTP_WORKER_H

