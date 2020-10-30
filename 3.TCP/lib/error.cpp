#include "error.h"
#include <stdexcept>

using namespace tcp;

AddressError::AddressError(const std::string& what, const Address&  addr)
    : Error{what}, _addr(addr) {}

Address AddressError::address() const { return _addr; }

SocketError::SocketError(const std::string& what)
    : Error{what} {}

DescripterError::DescripterError(const std::string& what)
    : Error{what} {}