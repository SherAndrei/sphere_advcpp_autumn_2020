#include "tcperr.h"

namespace tcp {

AddressError::AddressError(const std::string& what, const Address&  addr)
    : Error{what}, _addr(addr) {}

Address AddressError::address() const {
    return _addr;
}

SocketOptionError::SocketOptionError(const std::string& what,
                                          const std::string& option_name)
    : Error{what}, _opt(option_name) {}

std::string SocketOptionError::option() const {
    return _opt;
}

}  // namespace tcp
