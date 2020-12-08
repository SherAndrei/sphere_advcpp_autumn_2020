#ifndef HTTP_CONTAINERS_H
#define HTTP_CONTAINERS_H
#include <list>
#include <queue>
#include "client_container.h"

namespace http {

using TimeOrderedClients    = net::ClientPlaces;
using PlacesOfClosedClients = std::queue<net::IClientPlace, net::ClientPlaces>;

}  // namespace http

#endif  // HTTP_CONTAINERS_H
