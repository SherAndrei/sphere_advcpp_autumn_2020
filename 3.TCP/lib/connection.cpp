#include <arpa/inet.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <cstring>
#include "connection.h"
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

tcp::Connection::Connection(const Address& addr)
    : c_addr(addr) {
    connect(addr);
}
tcp::Connection::Connection(Socket && fd, const Address& addr)
    : c_addr(addr)
    , c_sock(std::move(fd))
{}

tcp::Connection::Connection(Connection && other)
    : c_addr(std::move(other.c_addr))
    , c_sock(std::move(other.c_sock))
{}

void tcp::Connection::connect(const Address& addr) {
    int error;

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port   = ::htons(addr.port());
    error = ::inet_aton(addr.address().data(), &sock_addr.sin_addr);
    if (error == 0)
        throw AddressError("incorrect address", addr);

    Socket s(AF_INET, SOCK_STREAM, 0);
    error = ::connect(s.fd(), reinterpret_cast<sockaddr*>(&sock_addr),
                      sizeof(sock_addr));
    if (error == -1)
        throw AddressError(std::strerror(errno), addr);

    c_sock = std::move(s);
}

size_t tcp::Connection::write(const void* data, size_t len) {
    ssize_t size;
    handle_error(size = ::write(c_sock.fd(), data, len));

    return static_cast<size_t> (size);
}
void tcp::Connection::writeExact(const void* data, size_t len) {
    size_t counter = 0u;
    const char* ch_data = static_cast<const char*> (data);
    while (counter < len)
        counter += write(ch_data + counter, len - counter);
}
size_t tcp::Connection::read(void* data, size_t len) {
    ssize_t size;
    handle_error(size = ::read(c_sock.fd(), data, len));

    return static_cast<size_t> (size);
}
void tcp::Connection::readExact(void* data, size_t len) {
    size_t counter = 0u;
    size_t current = 0u;
    char* ch_data = static_cast<char*> (data);
    while (counter < len) {
        current  = read(ch_data + counter, len - counter);
        if (current == 0u)
            throw DescriptorError("readExact failure");
        counter += current;
    }
}

void tcp::Connection::close() { c_sock.close(); }
void tcp::Connection::set_timeout(ssize_t sec, ssize_t usec) const {
    timeval timeout = { sec, usec };
    handle_error(setsockopt(c_sock.fd(), SOL_SOCKET, SO_SNDTIMEO,
                            &timeout, sizeof(timeout)));
    handle_error(setsockopt(c_sock.fd(), SOL_SOCKET, SO_RCVTIMEO,
                            &timeout, sizeof(timeout)));
}

void tcp::Connection::set_nonblock() const {
    int flags;
    handle_error(flags = fcntl(c_sock.fd(), F_GETFL));
    handle_error(fcntl(c_sock.fd(), F_SETFL, flags | O_NONBLOCK));
}

tcp::Connection& tcp::Connection::operator= (tcp::Connection && other) {
    this->c_addr = std::move(other.c_addr);
    this->c_sock = std::move(other.c_sock);
    return *this;
}

int tcp::Connection::fd() const {
    return c_sock.fd();
}
