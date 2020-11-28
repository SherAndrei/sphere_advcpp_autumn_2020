#ifndef NET_CONNECTION_MANAGER_H
#define NET_CONNECTION_MANAGER_H
#include <list>
#include "bufconnection.h"

namespace net {

using ConnectionManager = typename std::list<net::BufferedConnection>;

}  // namespace net


#endif  // NET_CONNECTION_MANAGER_H
