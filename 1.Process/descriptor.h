#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H
#include <unistd.h>

class Descriptor
{
public:
	bool isValid() const;

public:
	Descriptor() = default;
	explicit Descriptor(pid_t pid);
	~Descriptor();
	
	Descriptor(const Descriptor& other);
public:
	pid_t  operator()() const;
	pid_t operator()(pid_t pid);
private:
	pid_t _pid;
};

#endif //DESCRIPTOR_H