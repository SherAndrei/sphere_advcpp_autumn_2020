#ifndef NET_CONNECTION_CONTAINER_H
#define NET_CONNECTION_CONTAINER_H
#include <list>
#include <memory>
#include "iConnection.h"

namespace net {

using ConnectionUPtr = typename std::unique_ptr<tcp::IConnection>;

struct ConnectionPlace {
    ConnectionUPtr* p_conn;
    std::list<ConnectionPlace>::iterator iter{};
};

using ConnectionContainer = typename std::list<ConnectionUPtr>;
using ConnectionPlaces    = typename std::list<ConnectionPlace>;

}  // namespace net


#endif  // NET_CONNECTION_CONTAINER_H
