#ifndef NET_BASE_SERVICE_H
#define NET_BASE_SERVICE_H
#include "server.h"
#include "epoll.h"

namespace net {

class BaseService {
 protected:
    BaseService() = default;

 public:
    virtual ~BaseService() = default;

    BaseService(BaseService&& other)            = default;
    BaseService& operator=(BaseService&& other) = default;

 public:
    virtual void open(const tcp::Address& addr) = 0;
    virtual void run() = 0;
    virtual void close() = 0;

 private:
    BaseService(BaseService& other)             = delete;
    BaseService& operator=(BaseService&  other) = delete;

 protected:
    tcp::Server server_;
    EPoll epoll_;
};

}  // namespace net

#endif  // NET_BASE_SERVICE_H
