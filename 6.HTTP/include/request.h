#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <vector>
#include "header.h"

namespace http {

/// A request received from a client.
struct Request {
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<Header> headers;
};

}  // namespace http

#endif  // HTTP_REQUEST_H
