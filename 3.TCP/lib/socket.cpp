#include <cstring>
#include "tcperr.h"
#include "socket.h"

tcp::Socket::Socket(int fd)
    : fd_(fd) {
    if (!fd_.valid())
        throw tcp::SocketError("invalid socket");
}

tcp::Socket::Socket(int domain, int type, int protocol)
    : fd_(::socket(domain, type, protocol)) {
    if (!fd_.valid())
        throw tcp::SocketError(std::strerror(errno));
}

int  tcp::Socket::fd() const {
    return fd_.fd();
}
void tcp::Socket::open(int fd) {
    if (fd < 0)
        throw tcp::SocketError("invalid socket");
    close();
    fd_.set_fd(fd);
}
void tcp::Socket::open(int domain, int type, int protocol) {
    Descriptor temp(::socket(domain, type, protocol));
    if (!temp.valid())
        throw tcp::SocketError(std::strerror(errno));
    fd_ = std::move(temp);
}
void tcp::Socket::close() {
    fd_.close();
}
