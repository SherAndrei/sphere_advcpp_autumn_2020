#include "descriptor.h"

bool tcp::Descriptor::valid() const { return _id > -1; }
void tcp::Descriptor::invalidate()  { _id = -1; }

tcp::Descriptor::Descriptor(int id)              : _id(id)        {}
tcp::Descriptor::Descriptor(Descriptor && other) : _id(other._id) {
    other.invalidate();
}
tcp::Descriptor::~Descriptor() { close(); }

void tcp::Descriptor::close() {
    if (valid()) {
        ::close(_id);
        invalidate();
    }
}

int  tcp::Descriptor::fd() const     { return _id; }
void tcp::Descriptor::set_fd(int id) { _id = id; }

tcp::Descriptor& tcp::Descriptor::operator= (Descriptor && other) {
    close();
    _id = other._id;
    other.invalidate();
    return *this;
}
