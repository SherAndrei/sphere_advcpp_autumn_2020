#include "socket.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <cstring>
#include "tcperr.h"

namespace tcp {

void Socket::set_sndtimeo(ssize_t sec, ssize_t usec) const {
    timeval timeout = { sec, usec };
    int error = setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO,
                   &timeout, sizeof(timeout));
    if (error == -1) {
        throw SocketOptionError(std::strerror(errno), "SO_SNDTIMEO");
    }
}

void Socket::set_rcvtimeo(ssize_t sec, ssize_t usec) const {
    timeval timeout = { sec, usec };
    int error = setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout));
    if (error == -1) {
        throw SocketOptionError(std::strerror(errno), "SO_RCVTIMEO");
    }
}

void Socket::set_reuseaddr() const {
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        throw SocketOptionError(std::strerror(errno), "SO_REUSEADDR");
    }
}

void Socket::set_nonblock() const {
    int flags;
    if ((flags = fcntl(_fd, F_GETFL)) == -1) {
        throw SocketOptionError(std::strerror(errno), "O_NONBLOCK");
    }
    if ((fcntl(_fd, F_SETFL, flags | O_NONBLOCK)) == -1) {
        throw SocketOptionError(std::strerror(errno), "O_NONBLOCK");
    }
}

}  // namespace tcp
