#ifndef TCP_ADDRESS_H
#define TCP_ADDRESS_H
#include <ostream>
#include <string>

namespace tcp {

class Address {
 public:
    Address()  = default;
    Address(const std::string& addr, const uint16_t port);

    Address(const Address& other)            = default;
    Address& operator=(const Address& other) = default;

    Address(Address&& other);
    Address& operator=(Address&& other);

    ~Address() = default;

 public:
    uint16_t    port()    const;
    std::string address() const;
    std::string str()     const;

 private:
    std::string _address = {};
    uint16_t    _port    = 0u;
};

}  // namespace tcp

std::ostream& operator<<(std::ostream&, const tcp::Address&);

#endif  // TCP_ADDRESS_H
