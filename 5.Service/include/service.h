#ifndef NET_SERVICE_H
#define NET_SERVICE_H
#include "address.h"
#include "epoll.h"
#include "server.h"
#include "connection_manager.h"
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
    IServiceListener* listener_{nullptr};
 protected:
    tcp::Server server_;
    ConnectionManager manager_;
    EPoll epoll_;
};

}  // namespace net

#endif  // NET_SERVICE_H
