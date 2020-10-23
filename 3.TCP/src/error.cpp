#include "error.h"
#include <stdexcept>

using namespace tcp;

AddressError::AddressError(const std::string& what, const std::string&  addr, const uint16_t port)
    : Error{what}, _addr(addr), _port(port) {}

std::string   AddressError::addr() const { return _addr; }
std::uint16_t AddressError::port() const { return _port; }
