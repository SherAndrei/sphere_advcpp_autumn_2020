#ifndef NET_SERVICE_H
#define NET_SERVICE_H
#include <list>
#include <memory>
#include "address.h"
#include "epoll.h"
#include "server.h"
#include "bufconnection.h"
#include "listener.h"

namespace net {

class Service {
 public:
    explicit Service(IServiceListener* listener);
    void setListener(IServiceListener* listener);
    virtual ~Service() = default;
    virtual void open(const tcp::Address& addr);
    virtual void run();
    virtual void close();
 private:
    IServiceListener* listener_;
 protected:
    std::list<net::BufferedConnection> connections_;
    tcp::Server server_;
    net::EPoll epoll_;
};

}  // namespace net

#endif  // NET_SERVICE_H
