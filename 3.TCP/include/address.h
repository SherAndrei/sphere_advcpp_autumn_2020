#ifndef TCP_ADDRESS_H
#define TCP_ADDRESS_H
#include <string>

namespace tcp
{

class Address {
public:
    Address() = default;
    Address(const std::string& addr, const uint16_t port);
    Address(const Address&  other);
    Address(Address&& other);
    ~Address() = default;

    std::string address() const;
    uint16_t    port()    const;

    Address& operator= (const Address &  other);
    Address& operator= (Address && other);
private:
    std::string _addr;
    uint16_t    _port;
};

} // namespace tcp


#endif // TCP_ADDRESS_H