#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP
#include <sys/socket.h>
#include "descriptor.h"

namespace tcp {

class Socket {
 public:
    Socket() = default;
    explicit Socket(int fd);
    Socket(int domain, int type, int protocol);

    Socket(const Socket& rhs) = delete;
    Socket& operator=(const Socket& rhs) = delete;

    Socket(Socket&& rhs) = default;
    Socket& operator=(Socket&& rhs) = default;

    ~Socket() = default;

    int  fd() const;
    void open(int fd);
    void open(int domain, int type, int protocol);
    void close();

 private:
    Descriptor fd_;
};

}  // namespace tcp

#endif  // TCP_SOCKET_HPP
