#ifndef HTTP_CONTAINERS_H
#define HTTP_CONTAINERS_H
#include <list>
#include <queue>
#include "connection_container.h"

namespace http {

using TimeOrderedConnections    = net::ConnectionPlaces;
using PlacesOfClosedConnections = std::queue<net::ConnectionPlace, std::list<net::ConnectionPlace>>;

}  // namespace http

#endif  // HTTP_CONTAINERS_H
