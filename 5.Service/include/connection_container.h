#ifndef NET_CONNECTION_CONTAINER_H
#define NET_CONNECTION_CONTAINER_H
#include <list>
#include <memory>
#include <mutex>
#include "iConnection.h"

namespace net {

using ConnectionUPtr = typename std::unique_ptr<tcp::IConnection>;

struct ConnectionAndData {
 public:
    explicit ConnectionAndData(ConnectionUPtr conn)
        : u_conn(std::move(conn)) {}

    ConnectionUPtr u_conn;
    std::list<ConnectionAndData*>::iterator timeout_iter{};
    std::mutex timeout_mutex{};
};

}  // namespace net


#endif  // NET_CONNECTION_CONTAINER_H
