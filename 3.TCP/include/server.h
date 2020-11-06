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

 private:
    friend class Service;
    Server(Socket&& sock, const Address& addr);

 private:
    Address s_addr;
    Socket  s_sock;
};

}  // namespace tcp

#endif  // TCP_SERVER_H
