#include <algorithm>
#include "connection_manager.h"

namespace net {

void ConnectionManager::emplace(std::shared_ptr<BufferedConnection>&& other) {
    connections_.emplace_back(std::move(other));
}

ConnectionManager::iterator ConnectionManager::find(int fd) {
    return std::find_if(connections_.begin(), connections_.end(),
                [fd](std::shared_ptr<BufferedConnection>& client) {
                    return client->fd().fd() == fd;
                });
}

BufferedConnection& ConnectionManager::last() {
    return *(connections_.back());
}

void ConnectionManager::erase(iterator iter) {
    connections_.erase(iter);
}

}  // namespace net
