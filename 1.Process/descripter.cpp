#include "descripter.h"
#include <unistd.h>

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

pid_t Descripter::id() const
{
	return _id;
}

void Descripter::setID(int id)
{
	_id = id;
}

