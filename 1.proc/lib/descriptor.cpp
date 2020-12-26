#include "descriptor.h"

namespace prc {

bool Descriptor::valid() const { return _id > -1; }
void Descriptor::invalidate()  { _id = -1; }

Descriptor::Descriptor(int id)              : _id(id)        {}
Descriptor::Descriptor(Descriptor && other) : _id(other._id) {
    other.invalidate();
}
Descriptor::~Descriptor() { close(); }

void Descriptor::close() {
    if (valid()) {
        ::close(_id);
        invalidate();
    }
}

int  Descriptor::fd() const     { return _id; }
void Descriptor::set_fd(int id) {
    close();
    _id = id;
}

Descriptor& Descriptor::operator= (Descriptor && other) {
    close();
    _id = other._id;
    other.invalidate();
    return *this;
}

}  // namespace prc
