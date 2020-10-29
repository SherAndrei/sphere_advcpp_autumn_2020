#include "descripter.h"
#include <unistd.h>

using namespace tcp;

bool Descripter::valid() const { return _id != -1; }
void Descripter::invalidate()  { _id = -1; }

Descripter::Descripter(int id)              : _id(id)        {}
Descripter::Descripter(Descripter && other) : _id(other._id) { other.invalidate(); }
Descripter::~Descripter() { Descripter::close(); }

void Descripter::close()
{
	if(valid()) {
		::close(_id);
		invalidate();
	}	
}

int  Descripter::fd() const     { return _id; }
void Descripter::set_fd(int id) { _id = id; }

Descripter& Descripter::operator= (Descripter && other)
{
    _id = other._id;
    other.invalidate();
    return *this;
}