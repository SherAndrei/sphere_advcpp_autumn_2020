#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include "descriptor.h"
#include "connection.h"
#include "address.h"

namespace tcp {

class Server {
 public:
    Server() = default;
    explicit Server(const Address& addr);

    Server(const Server&  other)             = delete;
    Server& operator= (const Server&  other) = delete;

    Server(Server&& other)             = default;
    Server& operator= (Server&& other) = default;

    ~Server() = default;

 public:
    void listen(const Address& addr);
    Connection accept();
    Connection accept_non_block();

    void close();
    void set_timeout(ssize_t sec, ssize_t usec = 0l) const;
    void set_nonblock() const;
    void set_reuseaddr() const;

 public:
    Address address() const;

    Descriptor& socket();
    const Descriptor& socket() const;

 private:
    Address    address_;
    Descriptor socket_;
};

}  // namespace tcp

#endif  // TCP_SERVER_H
