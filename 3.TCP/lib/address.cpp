#include "address.h"

using namespace tcp;

Address::Address(const std::string& addr, const uint16_t port)
    : _addr(addr), _port(port) {}
Address::Address(const Address&  other) 
    : _addr(other._addr), _port(other._port) {}
Address::Address(Address&& other) 
    : _addr(std::move(other._addr)), _port(std::move(other._port)) {}

std::string Address::address() const { return _addr; }
uint16_t    Address::port()    const { return _port; }

Address& Address::operator= (const Address &  other) 
{
    this->_addr = other._addr;
    this->_port = other._port; 
    return *this;
}
Address& Address::operator= (Address && other) 
{
    this->_addr = std::move(other._addr);
    this->_port = std::move(other._port); 
    return *this;
}
