#ifndef TCP_ADDRESS_H
#define TCP_ADDRESS_H
#include <string>

namespace tcp
{

class Address {
public:
    Address()  = default;
    Address(const std::string& addr, const uint16_t port); 
    
    ~Address() = default;
    
    Address(const Address& other)            = default;
    Address& operator=(const Address& other) = default;

    Address(Address&& other);
    Address& operator=(Address&& other);

    uint16_t    port()    const;
    std::string address() const;

private:
    std::string _address = {};
    uint16_t    _port    = 0u;
};



} // namespace tcp


#endif // TCP_ADDRESS_H