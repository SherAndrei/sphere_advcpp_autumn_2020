#ifndef TCP_I_CONNECTION_H
#define TCP_I_CONNECTION_H
#include "descriptor.h"
#include "address.h"

namespace tcp {

class IConnectable {
 protected:
    IConnectable()  = default;
    explicit IConnectable(const Address& addr)
            : address_(addr) {}

 public:
    virtual ~IConnectable() = default;

    IConnectable(IConnectable && other)             = default;
    IConnectable& operator= (IConnectable && other) = default;

    virtual void close() = 0;

 private:
    IConnectable(const IConnectable& other)               = delete;
    IConnectable& operator= (const IConnectable &  other) = delete;

 protected:
    void set_socket(Descriptor&& other) {
        socket_ = std::move(other);
    }

 public:
    Address address() const { return address_; }

    Descriptor&       socket()       { return socket_; }
    const Descriptor& socket() const { return socket_; }

 protected:
    Address    address_;
    Descriptor socket_;
};

}  // namespace tcp

#endif  // TCP_I_CONNECTION_H
