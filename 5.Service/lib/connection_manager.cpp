#include <algorithm>
#include "connection_manager.h"

namespace net {

void ConnectionManager::emplace(tcp::Connection&& c, EPoll* p_epoll) {
    connections_.emplace_back(std::move(c), p_epoll);
}

ConnectionManager::iterator ConnectionManager::find(int fd) {
    return std::find_if(connections_.begin(), connections_.end(),
                       [fd](BufferedConnection& client) {
                           return client.fd().fd() == fd;
                       });
}
BufferedConnection& ConnectionManager::last() {
    return connections_.back();
}

void ConnectionManager::erase(iterator iter) {
    connections_.erase(iter);
}

}  // namespace net
