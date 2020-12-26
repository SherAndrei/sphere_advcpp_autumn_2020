#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H
#include "descriptor.h"

namespace tcp {

class Socket : public Descriptor {
 public:
    using Descriptor::Descriptor;

 public:
    void set_sndtimeo(ssize_t sec, ssize_t usec = 0l) const;
    void set_rcvtimeo(ssize_t sec, ssize_t usec = 0l) const;
    void set_nonblock() const;
    void set_reuseaddr() const;
};

}  // namespace tcp

#endif  // TCP_SOCKET_H
