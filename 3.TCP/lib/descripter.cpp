#include "descripter.h"
#include <unistd.h>

using namespace tcp;

bool Descripter::isValid() const
{
	return _id != -1;
}

Descripter::Descripter(int id) 
	: _id(id)
{}

Descripter::~Descripter() 
{
	Descripter::close();
}

void Descripter::close()
{
	if(isValid()) {
		::close(_id);
		_id = -1;
	}	
}

pid_t Descripter::fd() const
{
	return _id;
}

void Descripter::set_fd(int id)
{
	_id = id;
}

