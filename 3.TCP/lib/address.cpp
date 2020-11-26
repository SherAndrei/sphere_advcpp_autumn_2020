#include <utility>
#include "address.h"

namespace tcp {

Address::Address(const std::string& addr, const uint16_t port)
    : _address(addr), _port(port) {}

uint16_t Address::port() const {
    return _port;
}

std::string Address::address() const {
    return _address;
}

Address::Address(Address&& other) {
    _address = std::move(other._address);
    _port = std::exchange(other._port, 0u);
}

Address& Address::operator=(Address&& other) {
    _address = std::move(other._address);
    _port = std::exchange(other._port, 0u);
    return *this;
}

}  // namespace tcp

std::ostream& operator<<(std::ostream& os, const tcp::Address& addr) {
    return os << addr.address() << ':' << addr.port();
}
