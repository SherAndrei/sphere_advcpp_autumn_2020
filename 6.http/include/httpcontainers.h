#ifndef HTTP_CONTAINERS_H
#define HTTP_CONTAINERS_H
#include <list>
#include <queue>
#include "connection_container.h"

namespace http {

using TimeOrderedConnections    = std::list<net::ConnectionAndData*>;
using PlacesOfClosedConnections = std::queue<net::ConnectionAndData*, std::list<net::ConnectionAndData*>>;

}  // namespace http

#endif  // HTTP_CONTAINERS_H
