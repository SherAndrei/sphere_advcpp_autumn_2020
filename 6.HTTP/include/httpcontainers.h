#ifndef HTTP_CONTAINERS_H
#define HTTP_CONTAINERS_H
#include <list>
#include <queue>
#include "client_container.h"

namespace http {

using PtrsToClosedClients = std::queue<net::IClient*, std::list<net::IClient*>>;

}  // namespace http

#endif  // HTTP_CONTAINERS_H
