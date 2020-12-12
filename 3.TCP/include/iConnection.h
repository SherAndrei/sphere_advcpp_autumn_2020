#ifndef TCP_I_CONNECTION_H
#define TCP_I_CONNECTION_H
#include "socket.h"
#include "address.h"

namespace tcp {

class IConnection {
 protected:
    IConnection()  = default;
    explicit IConnection(const Address& addr)
            : address_(addr) {}

 public:
    virtual ~IConnection() = default;

    IConnection(IConnection && other)             = default;
    IConnection& operator= (IConnection && other) = default;

    virtual void close() = 0;

 private:
    IConnection(const IConnection& other)               = delete;
    IConnection& operator= (const IConnection &  other) = delete;

 protected:
    void set_socket(Socket&& other) {
        socket_ = std::move(other);
    }

 public:
    Address address() const { return address_; }

    Socket&       socket()       { return socket_; }
    const Socket& socket() const { return socket_; }

 protected:
    Address address_;
    Socket  socket_;
};

}  // namespace tcp

#endif  // TCP_I_CONNECTION_H
