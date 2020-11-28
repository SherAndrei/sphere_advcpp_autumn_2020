#ifndef NET_SERVICE_H
#define NET_SERVICE_H
#include "address.h"
#include "listener.h"
#include "server.h"
#include "connection_manager.h"
#include "epoll.h"

namespace net {

class Service {
 public:
    explicit Service(IServiceListener* listener);
    void setListener(IServiceListener* listener);
    void open(const tcp::Address& addr);
    void run();
    void close();
 private:
    IServiceListener* listener_{nullptr};
    tcp::Server server_;
    ConnectionManager manager_;
    EPoll epoll_;
};

}  // namespace net

#endif  // NET_SERVICE_H
