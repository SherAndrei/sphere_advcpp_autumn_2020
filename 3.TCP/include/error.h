#ifndef TCP_ERROR_H
#define TCP_ERROR_H
#include <stdexcept>
#include "address.h"

namespace tcp
{
class Error : public std::runtime_error {
public: 
    using std::runtime_error::runtime_error;
};

class DescripterError : public Error {
public:
    DescripterError(const std::string& what);
};

class SocketError : public Error {
public:
    SocketError(const std::string& what);
};

class AddressError : public Error {
private:
    const Address _addr;
public:
    AddressError(const std::string& what, const Address& addr);

    Address address() const;
};

} // namespace tcp

#endif // TCP_ERROR_H