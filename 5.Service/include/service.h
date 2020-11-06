#ifndef NET_SERVICE_H
#define NET_SERVICE_H
#include <list>
#include <memory>
#include "address.h"
#include "epoll.h"
#include "server.h"
#include "connection.h"
#include "listener.h"

namespace net {

class Service {
 public:
    explicit Service(std::shared_ptr<IServiceListener> listener);
    void setListener(std::shared_ptr<IServiceListener> listener);
    void open(const tcp::Address& addr);
    void run();
    void close();
    void closeConnection(const BufferedConnection& bc);
    void subscribeTo(const BufferedConnection& bc  /*, read and or write*/);
    void unsubsribeFrom(const BufferedConnection& bc  /*, read and or write*/);
 private:
    std::shared_ptr<IServiceListener> listener_;
    std::list<tcp::Connection> connections_;
    tcp::Server server_;
    net::EPoll epoll_;
};

}  // namespace net

#endif  // NET_SERVICE_H
