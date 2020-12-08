#ifndef NET_BASE_SERVICE_H
#define NET_BASE_SERVICE_H
#include "server.h"
#include "epoll.h"
#include "client_container.h"

namespace net {

class IService {
 protected:
    IService() = default;
    explicit IService(const tcp::Address& addr)
            : server_(addr) {}

 public:
    virtual ~IService() = default;

    IService(IService&& other)            = default;
    IService& operator=(IService&& other) = default;

 public:
    virtual void open(const tcp::Address& addr) = 0;
    virtual void run() = 0;
    virtual void close() = 0;

 private:
    IService(IService& other)             = delete;
    IService& operator=(IService&  other) = delete;

 protected:
    tcp::Server server_;
    EPoll epoll_;
    ClientContainer clients_;
};

}  // namespace net

#endif  // NET_BASE_SERVICE_H
