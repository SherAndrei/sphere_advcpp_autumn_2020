#ifndef NET_CONNECTION_MANAGER_H
#define NET_CONNECTION_MANAGER_H
#include <list>
#include <memory>
#include "bufconnection.h"

namespace net {

class ConnectionManager {
    using iterator = typename std::list<std::shared_ptr<BufferedConnection>>::iterator;

 public:
    void emplace(std::shared_ptr<BufferedConnection>&& other);
    iterator find(int fd);
    BufferedConnection& last();
    void erase(iterator iter);
 private:
    std::list<std::shared_ptr<BufferedConnection>> connections_;
};

}  // namespace net


#endif  // NET_CONNECTION_MANAGER_H
