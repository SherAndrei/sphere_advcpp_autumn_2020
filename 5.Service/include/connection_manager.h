#ifndef NET_CONNECTION_MANAGER_H
#define NET_CONNECTION_MANAGER_H
#include <list>
#include "bufconnection.h"

namespace net {

class ConnectionManager {
    using iterator = std::list<net::BufferedConnection>::iterator;
 public:
    void emplace(tcp::Connection&& c, EPoll* p_epoll);
    iterator find(int fd);
    BufferedConnection& last();
    void erase(iterator iter);
 private:
    std::list<net::BufferedConnection> connections_;
};

}  // namespace net


#endif  // NET_CONNECTION_MANAGER_H
