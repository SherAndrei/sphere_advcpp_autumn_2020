#include "nonblock_connection.h"
#include <cstring>
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

NonBlockConnection::NonBlockConnection(Descriptor&& fd, const Address& addr)
    : IConnectable(addr) {
    set_socket(std::move(fd));
}

void NonBlockConnection::close() {
    socket_.close();
    address_ = {{}, 0u};
}

size_t NonBlockConnection::write(const std::string& data) {
    ssize_t size;
    handle_error(size = ::write(socket_.fd(), data.data(), data.length()));

    return static_cast<size_t> (size);
}

size_t NonBlockConnection::read(std::string& data) {
    ssize_t size;
    handle_error(size = ::read(socket_.fd(), data.data(), data.length()));

    return static_cast<size_t> (size);
}
}  // namespace tcp
