#include "address.h"
#include <utility>
using namespace tcp;

Address::Address(const std::string& addr, const uint16_t port) 
    : _address(addr), _port(port) {}

uint16_t Address::port() const {
    return _port;
}

std::string Address::address() const {
    return _address;
}

Address::Address(Address&& other) {
    this->_address = std::move(other._address);
    this->_port = std::exchange(other._port, 0u);
}

Address& Address::operator=(Address&& other){
    this->_address = std::move(other._address);
    this->_port = std::exchange(other._port, 0u);
    return *this;
}