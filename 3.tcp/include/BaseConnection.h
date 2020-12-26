#ifndef TCP_BASE_CONNECTION_H
#define TCP_BASE_CONNECTION_H
#include "socket.h"
#include "address.h"

namespace tcp {

class BaseConnection {
 protected:
    BaseConnection()  = default;
    explicit BaseConnection(const Address& addr)
            : address_(addr) {}

 public:
    virtual ~BaseConnection() = default;

    BaseConnection(BaseConnection && other)             = default;
    BaseConnection& operator= (BaseConnection && other) = default;

    virtual void close() = 0;

 private:
    BaseConnection(const BaseConnection& other)               = delete;
    BaseConnection& operator= (const BaseConnection &  other) = delete;

 protected:
    void set_socket(Socket&& other) {
        socket_ = std::move(other);
    }

 public:
    Address address() const { return address_; }
    const Socket& socket() const { return socket_; }

 protected:
    Address address_;
    Socket  socket_;
};

}  // namespace tcp

#endif  // TCP_BASE_CONNECTION_H
