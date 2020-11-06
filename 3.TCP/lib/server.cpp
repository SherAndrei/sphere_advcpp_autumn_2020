#include <arpa/inet.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <cstring>
#include "server.h"
#include "tcperr.h"

static void handle_error(int errnum) {
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

tcp::Server::Server(const tcp::Address& addr)
    : s_addr(addr) {
    listen(addr);
}

tcp::Server::Server(tcp::Server&& other)
    : s_addr(std::move(other.s_addr))
    , s_sock(std::move(other.s_sock)) {}

void tcp::Server::listen(const tcp::Address& addr) {
    int error;

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(addr.port());
    error = ::inet_aton(addr.address().data(), &sock_addr.sin_addr);
    if (error == 0)
        throw tcp::AddressError("incorrect address", addr);
    Socket s(AF_INET, SOCK_STREAM, 0);

    error = ::bind(s.fd(), reinterpret_cast<sockaddr*>(&sock_addr),
                   sizeof(sock_addr));
    if (error == -1)
        throw tcp::AddressError(std::strerror(errno), addr);

    error = ::listen(s.fd(), SOMAXCONN);
    if (error == -1)
        throw tcp::AddressError(std::strerror(errno), addr);
    s_sock = std::move(s);
}
tcp::Connection tcp::Server::accept() {
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client;
    handle_error(client = ::accept(s_sock.fd(),
                                   reinterpret_cast<sockaddr*>(&peer_addr),
                                   &s));

    return Connection{ Socket{client},
                       Address{ ::inet_ntoa(peer_addr.sin_addr),
                                peer_addr.sin_port } };
}

void tcp::Server::close() {
    s_sock.close();
}

void tcp::Server::set_timeout(ssize_t sec, ssize_t usec) const {
    timeval timeout = { sec, usec };
    handle_error(setsockopt(s_sock.fd(), SOL_SOCKET, SO_SNDTIMEO,
                            &timeout, sizeof(timeout)));
    handle_error(setsockopt(s_sock.fd(), SOL_SOCKET, SO_RCVTIMEO,
                            &timeout, sizeof(timeout)));
}

void tcp::Server::set_nonblock() const {
    int flags;
    handle_error(flags = fcntl(s_sock.fd(), F_GETFL));
    handle_error(fcntl(s_sock.fd(), F_SETFL, flags | O_NONBLOCK));
}

tcp::Server& tcp::Server::operator= (tcp::Server&& other) {
    this->s_addr = std::move(other.s_addr);
    this->s_sock = std::move(other.s_sock);
    return *this;
}

int tcp::Server::fd() const {
    return s_sock.fd();
}
