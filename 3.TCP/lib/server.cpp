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
    : BaseConnection(addr) {
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

    Socket s(::socket(AF_INET, SOCK_STREAM, 0));
    if (!s.valid()) {
        throw SocketError(std::strerror(errno));
    }
    s.set_reuseaddr();
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

    return Connection{ Socket{client},
                       Address{ ::inet_ntoa(peer_addr.sin_addr),
                                peer_addr.sin_port } };
}

NonBlockConnection Server::accept_non_block() {
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client;
    client = ::accept4(socket_.fd(), reinterpret_cast<sockaddr*>(&peer_addr),
                      &s, SOCK_NONBLOCK);
    handle_error(client);
    return NonBlockConnection{ Socket{client},
                               Address{ ::inet_ntoa(peer_addr.sin_addr),
                                        peer_addr.sin_port } };
}

void Server::close() {
    socket_.close();
}

}  // namespace tcp
