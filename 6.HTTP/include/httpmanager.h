#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H
#include <list>
#include <queue>
#include "httpconnection.h"

namespace http {

using HttpManager = std::list<HttpConnection>;

}  // namespace http

#endif  // HTTP_MANAGER_H
