#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include "socket.h"
#include "connection.h"
#include "address.h"

namespace tcp {

class Server {
 public:
    Server() = default;
    explicit Server(const Address& addr);
    Server(const Address& addr, Socket&& s = Socket{AF_INET, SOCK_STREAM, 0});

    Server(const Server&  other) = delete;
    Server& operator= (const Server&  other) = delete;

    Server(Server&& other);
    Server& operator= (Server&& other);

    ~Server() = default;

 public:
    void listen(const Address&, Socket&& s = Socket{AF_INET, SOCK_STREAM, 0});
    Connection accept();

    void close();
    void set_timeout(ssize_t sec, ssize_t usec = 0l) const;

 public:
    int fd() const;
 private:
    Address s_addr;
    Socket  s_sock;
};

}  // namespace tcp

#endif  // TCP_SERVER_H
