#include <arpa/inet.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <cstring>
#include "connection.h"
#include "tcperr.h"

namespace {

void handle_error(int errnum) {
    if (errnum == -1) {
        if (errno == EAGAIN ||
            errno == EWOULDBLOCK ||
            errno == EINPROGRESS) {
            throw tcp::TimeOutError(std::strerror(errno));
        } else if (errno == EBADF) {
            throw tcp::DescriptorError(std::strerror(errno));
        } else {
            throw tcp::Error(std::strerror(errno));
        }
    }
}

}  // namespace

namespace tcp {

Connection::Connection(const Address& addr)
    : IConnection(addr) {
    connect(addr);
}

Connection::Connection(Socket&& fd, const Address& addr)
    : IConnection(addr) {
    set_socket(std::move(fd));
}

void Connection::connect(const Address& addr) {
    int error;

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port   = ::htons(addr.port());
    error = ::inet_aton(addr.address().data(), &sock_addr.sin_addr);
    if (error == 0)
        throw AddressError("incorrect address", addr);

    Socket s(::socket(AF_INET, SOCK_STREAM, 0));
    if (!s.valid()) {
        throw SocketError(std::strerror(errno));
    }
    error = ::connect(s.fd(), reinterpret_cast<sockaddr*>(&sock_addr),
                      sizeof(sock_addr));
    if (error == -1)
        throw AddressError(std::strerror(errno), addr);

    socket_ = std::move(s);
}

size_t Connection::write(const void* data, size_t len) {
    ssize_t size;
    handle_error(size = ::write(socket_.fd(), data, len));

    return static_cast<size_t> (size);
}
void Connection::writeExact(const void* data, size_t len) {
    size_t counter = 0u;
    const char* ch_data = static_cast<const char*> (data);
    while (counter < len)
        counter += write(ch_data + counter, len - counter);
}
size_t Connection::read(void* data, size_t len) {
    ssize_t size;
    handle_error(size = ::read(socket_.fd(), data, len));

    return static_cast<size_t> (size);
}
void Connection::readExact(void* data, size_t len) {
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

void Connection::close() {
    socket_.close();
    address_ = {{}, 0u};
}

}  // namespace tcp
