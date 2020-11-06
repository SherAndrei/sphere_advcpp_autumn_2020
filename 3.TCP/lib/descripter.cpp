#include "descripter.h"

bool tcp::Descripter::valid() const { return _id != -1; }
void tcp::Descripter::invalidate()  { _id = -1; }

tcp::Descripter::Descripter(int id)              : _id(id)        {}
tcp::Descripter::Descripter(Descripter && other) : _id(other._id) {
    other.invalidate();
}
tcp::Descripter::~Descripter() { close(); }

void tcp::Descripter::close() {
    if (valid()) {
        ::close(_id);
        invalidate();
    }
}

int  tcp::Descripter::fd() const     { return _id; }
void tcp::Descripter::set_fd(int id) { _id = id; }

tcp::Descripter& tcp::Descripter::operator= (Descripter && other) {
    this->close();
    this->_id = other._id;
    other.invalidate();
    return *this;
}
