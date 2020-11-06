#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <cstring>
#include "descripter.h"
#include "connection.h"
#include "server.h"
#include "address.h"
#include "tcperr.h"

static void handle_error(int errnum) {
    if (errnum == -1)
        throw tcp::Error(std::strerror(errno));
}

tcp::Server::Server(const tcp::Address& addr)
    : s_addr(addr) {
    listen(addr);
}

tcp::Server::Server(tcp::Server&& other)
    : s_addr(std::move(other.s_addr))
    , s_sock(std::move(other.s_sock)) {}

tcp::Server::Server(tcp::Socket && sock, tcp::Address&& addr)
    : s_addr(std::move(addr))
    , s_sock(std::move(sock)) {}

void tcp::Server::listen(const tcp::Address& addr) {
    int error;
    // Для строгой гарантии исключений
    Socket temp(AF_INET, SOCK_STREAM, 0);

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(addr.port());
    error = ::inet_aton(addr.address().data(), &sock_addr.sin_addr);
    if (error == 0)
        throw tcp::AddressError("Incorrect address!", addr);

    // привязываем дескриптор к сокету
    error = ::bind(temp.fd(), reinterpret_cast<sockaddr*>(&sock_addr),
                   sizeof(sock_addr));
    if (error == -1)
        throw tcp::AddressError(std::strerror(errno), addr);

    error = ::listen(temp.fd(), SOMAXCONN);
    if (error == -1)
        throw tcp::AddressError(std::strerror(errno), addr);
    s_sock = std::move(temp);
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

tcp::Server& tcp::Server::operator= (tcp::Server&& other) {
    this->s_addr = std::move(other.s_addr);
    this->s_sock = std::move(other.s_sock);
    return *this;
}
