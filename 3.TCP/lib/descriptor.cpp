#include "descriptor.h"

namespace tcp {

bool Descriptor::valid() const { return _fd > -1; }
void Descriptor::invalidate()  { _fd = -1; }

Descriptor::Descriptor(int fd)              : _fd(fd)        {}
Descriptor::Descriptor(Descriptor && other) : _fd(other._fd) {
    other.invalidate();
}
Descriptor::~Descriptor() { close(); }

void Descriptor::close() {
    if (valid()) {
        ::close(_fd);
        invalidate();
    }
}

int  Descriptor::fd() const     { return _fd; }
void Descriptor::set_fd(int fd) {
    close();
    _fd = fd;
}

Descriptor& Descriptor::operator= (Descriptor && other) {
    close();
    _fd = other._fd;
    other.invalidate();
    return *this;
}

}  // namespace tcp
