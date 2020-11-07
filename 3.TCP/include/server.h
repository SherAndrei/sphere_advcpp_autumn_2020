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

    Server(const Server&  other) = delete;
    Server& operator= (const Server&  other) = delete;

    Server(Server&& other);
    Server& operator= (Server&& other);

    ~Server() = default;

 public:
    void listen(const Address& addr);
    Connection accept();

    void close();
    void set_timeout(ssize_t sec, ssize_t usec = 0l) const;
    void set_nonblock() const;
    void set_reuseaddr() const;

 public:
    Descriptor& fd();

 private:
    Address    s_addr;
    Descriptor s_sock;
};

}  // namespace tcp

#endif  // TCP_SERVER_H
