#ifndef NET_ERROR_H
#define NET_ERROR_H
#include <string>
#include <stdexcept>
#include "address.h"

namespace net {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class EPollError : public Error {
 public:
    explicit EPollError(const std::string& what);
};

}  // namespace net

#endif  // NET_ERROR_H
