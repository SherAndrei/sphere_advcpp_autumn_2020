#include "descriptor.h"

bool prc::Descriptor::valid() const { return _id > -1; }
void prc::Descriptor::invalidate()  { _id = -1; }

prc::Descriptor::Descriptor(int id)              : _id(id)        {}
prc::Descriptor::Descriptor(Descriptor && other) : _id(other._id) {
    other.invalidate();
}
prc::Descriptor::~Descriptor() { close(); }

void prc::Descriptor::close() {
    if (valid()) {
        ::close(_id);
        invalidate();
    }
}

int  prc::Descriptor::fd() const     { return _id; }
void prc::Descriptor::set_fd(int id) {
    close();
    _id = id;
}

prc::Descriptor& prc::Descriptor::operator= (Descriptor && other) {
    close();
    _id = other._id;
    other.invalidate();
    return *this;
}
