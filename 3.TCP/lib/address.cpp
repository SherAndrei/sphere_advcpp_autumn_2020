#include <utility>
#include "address.h"

tcp::Address::Address(const std::string& addr, const uint16_t port)
    : _address(addr), _port(port) {}

uint16_t tcp::Address::port() const {
    return _port;
}

std::string tcp::Address::address() const {
    return _address;
}

tcp::Address::Address(Address&& other) {
    _address = std::move(other._address);
    _port = std::exchange(other._port, 0u);
}

tcp::Address& tcp::Address::operator=(tcp::Address&& other) {
    _address = std::move(other._address);
    _port = std::exchange(other._port, 0u);
    return *this;
}

std::ostream& operator<<(std::ostream& os, const tcp::Address& addr) {
    return os << addr.address() << ':' << addr.port();
}
