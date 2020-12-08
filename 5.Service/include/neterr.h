#ifndef NET_ERROR_H
#define NET_ERROR_H
#include <stdexcept>

namespace net {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class EPollError : public Error {
 public:
    using Error::Error;
};

class ListenerError : public Error {
 public:
    using Error::Error;
};

}  // namespace net

#endif  // NET_ERROR_H
