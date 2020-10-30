#ifndef TCP_ADDRESS_H
#define TCP_ADDRESS_H
#include <string>

namespace tcp
{

struct Address {
    std::string address = {};
    uint16_t    port    = 0u;
};

} // namespace tcp


#endif // TCP_ADDRESS_H