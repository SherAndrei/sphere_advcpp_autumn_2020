#include "error.h"
#include <stdexcept>

namespace tcp
{

AddressError::AddressError(const std::string& what, const std::string&  addr, uint16_t port)
    : tcp::Error{what}, _addr(addr), _port(port) {}

std::string   AddressError::addr() const { return _addr; }
std::uint16_t AddressError::port() const { return _port; }

} // namespace tcp