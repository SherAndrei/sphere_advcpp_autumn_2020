#include "tcperr.h"

tcp::AddressError::AddressError(const std::string& what, const Address&  addr)
    : Error{what}, _addr(addr) {}

tcp::Address tcp::AddressError::address() const { return _addr; }

tcp::SocketError::SocketError(const std::string& what)
    : Error{what} {}

tcp::TimeOutError::TimeOutError(const std::string& what)
    : Error{what} {}

tcp::DescriptorError::DescriptorError(const std::string& what)
    : Error{what} {}
