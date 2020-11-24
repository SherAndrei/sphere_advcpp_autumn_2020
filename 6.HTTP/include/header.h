#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H
#include <string>

namespace http {

struct Header {
    std::string name;
    std::string value;
};

}  // namespace http

#endif  // HTTP_HEADER_H
