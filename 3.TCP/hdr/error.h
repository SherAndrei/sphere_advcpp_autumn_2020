#ifndef ERROR_H
#define ERROR_H
#include <stdexcept>

namespace tcp
{

class Error : public std::runtime_error {
public: 
    using std::runtime_error::runtime_error;
};

class AddressError : public Error {
private:
    std::string _addr;
    uint16_t    _port;
public:
    AddressError(const std::string& what, const std::string&  addr, const uint16_t port);

    std::string   addr() const;
    std::uint16_t port() const;
};


} // namespace tcp

#endif // ERROR_H