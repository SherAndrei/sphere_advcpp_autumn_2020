#include "tcperr.h"

tcp::AddressError::AddressError(const std::string& what, const Address&  addr)
    : Error{what}, _addr(addr) {}

tcp::Address tcp::AddressError::address() const { return _addr; }

tcp::SocketOptionError::SocketOptionError(const std::string& what,
                                          const std::string& option_name)
    : Error{what}, _opt(option_name) {}

std::string tcp::SocketOptionError::option() const {
    return _opt;
}
