#include <arpa/inet.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <cstring>
#include "server.h"
#include "tcperr.h"

namespace {

void handle_error(int errnum) {
    if (errnum == -1) {
        if (errno == EAGAIN ||
            errno == EWOULDBLOCK ||
            errno == EINPROGRESS) {
            throw tcp::TimeOutError(std::strerror(errno));
        } else {
            throw tcp::Error(std::strerror(errno));
        }
    }
}

}  // namespace

namespace tcp {

Server::Server(const Address& addr)
    : address_(addr) {
    listen(addr);
}

void Server::listen(const Address& addr) {
    int error;

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(addr.port());
    error = ::inet_aton(addr.address().data(), &sock_addr.sin_addr);
    if (error == 0)
        throw AddressError("incorrect address", addr);

    Descriptor s(::socket(AF_INET, SOCK_STREAM, 0));
    if (!s.valid()) {
        throw SocketError(std::strerror(errno));
    }
    error = ::bind(s.fd(), reinterpret_cast<sockaddr*>(&sock_addr),
                   sizeof(sock_addr));
    if (error == -1)
        throw AddressError(std::strerror(errno), addr);

    error = ::listen(s.fd(), SOMAXCONN);
    if (error == -1)
        throw AddressError(std::strerror(errno), addr);
    socket_ = std::move(s);
}
Connection Server::accept() {
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client;
    handle_error(client = ::accept(socket_.fd(),
                                   reinterpret_cast<sockaddr*>(&peer_addr),
                                   &s));

    return Connection{ Descriptor{client},
                       Address{ ::inet_ntoa(peer_addr.sin_addr),
                                peer_addr.sin_port } };
}

Connection Server::accept_non_block() {
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client;
    handle_error(client = ::accept4(socket_.fd(),
                                   reinterpret_cast<sockaddr*>(&peer_addr),
                                   &s, SOCK_NONBLOCK));

    return Connection{ Descriptor{client},
                       Address{ ::inet_ntoa(peer_addr.sin_addr),
                                peer_addr.sin_port } };
}

void Server::close() {
    socket_.close();
}

void Server::set_timeout(ssize_t sec, ssize_t usec) const {
    timeval timeout = { sec, usec };
    int error = setsockopt(socket_.fd(), SOL_SOCKET, SO_SNDTIMEO,
                   &timeout, sizeof(timeout));
    if (error == -1) {
        throw SocketOptionError(std::strerror(errno), "SO_SNDTIMEO");
    }
    error = setsockopt(socket_.fd(), SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout));
    if (error == -1) {
        throw SocketOptionError(std::strerror(errno), "SO_RCVTIMEO");
    }
}

void Server::set_nonblock() const {
    int flags;
    if ((flags = fcntl(socket_.fd(), F_GETFL)) == -1) {
        throw SocketOptionError(std::strerror(errno), "O_NONBLOCK");
    }
    if ((fcntl(socket_.fd(), F_SETFL, flags | O_NONBLOCK)) == -1) {
        throw SocketOptionError(std::strerror(errno), "O_NONBLOCK");
    }
}

void Server::set_reuseaddr() const {
    int opt = 1;
    if (setsockopt(socket_.fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        throw SocketOptionError(std::strerror(errno), "SO_REUSEADDR");
    }
}

Descriptor& Server::socket() {
    return socket_;
}

const Descriptor& Server::socket() const {
    return socket_;
}

Address Server::address() const {
    return address_;
}

}  // namespace tcp
