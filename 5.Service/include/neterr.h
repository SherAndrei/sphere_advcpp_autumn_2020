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

class DescripterError : public Error {
 public:
    explicit DescripterError(const std::string& what);
};

class SocketError : public Error {
 public:
    explicit SocketError(const std::string& what);
};

class AddressError : public Error {
 private:
    const tcp::Address _addr;
 public:
    AddressError(const std::string& what, const tcp::Address& addr);

    tcp::Address address() const;
};


}  // namespace net

#endif  // NET_ERROR_H
