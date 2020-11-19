#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include "bufconnection.h"

namespace http {

using HttpConnection = net::BufferedConnection;

}  // namespace http

#endif  // HTTP_CONNECTION_H
