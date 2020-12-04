#ifndef HTTP_COR_MANAGER_H
#define HTTP_COR_MANAGER_H
#include <list>
#include <queue>
#include "corconnection.h"

namespace http {
namespace cor {

using CorManager = std::list<CorConnection>;
using PtrsToClosedCorConnections = std::queue<CorConnection*, std::list<CorConnection*>>;

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_MANAGER_H
