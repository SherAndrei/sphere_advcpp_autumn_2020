#include "descriptor.h"

bool Descriptor::isValid() const
{
	return _pid != -1;
}

Descriptor::Descriptor(int pid) 
	: _pid(pid)
{}

Descriptor::~Descriptor() 
{
	if(isValid()) {
		::close(_pid);
		_pid = -1;
	}
}

pid_t Descriptor::operator()() const
{
	return _pid;
}

pid_t& Descriptor::operator()(pid_t pid)
{
	return (_pid = pid);
}