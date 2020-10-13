#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <thread>
#include <string>
#include <iostream>
#include <exception>
#include "process.h"

#define _THROW_RUNTIME_ERR(x) throw std::runtime_error(x)

Process::Process(const std::string& path)
{
	int pipe_in[2], pipe_out[2];
	if(pipe2(pipe_out, O_CLOEXEC) == -1)
		_THROW_RUNTIME_ERR("pipe from child");

	if(pipe(pipe_in) == -1) {
		::close(pipe_out[0]);
		::close(pipe_out[1]);
		_THROW_RUNTIME_ERR("pipe to child");
	}
	
	Descriptor w_pid_out(pipe_out[1]);
			  _r_pid_out(pipe_out[0]);

	Descriptor r_pid_in(pipe_in[0]);
			  _w_pid_in(pipe_in[1]);

	if((_cpid(fork())) == -1) 
		_THROW_RUNTIME_ERR("fork");

	if(_cpid() == 0) { /* child process */
		if(::dup2(r_pid_in(), STDIN_FILENO) == -1) /* Child now reads from replaced STDIN_FILENO */
			_THROW_RUNTIME_ERR("dup2 stdin");
		r_pid_in.~Descriptor();			   /* Now we do not need the original one*/
		
		if(::dup2(w_pid_out(), STDOUT_FILENO) == -1) /* same */
			_THROW_RUNTIME_ERR("dup2 stdout");
		w_pid_out.~Descriptor();
		
		if(execl(path.c_str(), path.c_str(), nullptr) == -1) /* executing.. */		
			_THROW_RUNTIME_ERR("execl");
    }
	// pipe_in[0] и pipe_out[1] закрываются через деструкторы
}

Process::~Process()
{
	_r_pid_out.~Descriptor();
	_w_pid_in.~Descriptor();
	try {
		close();
	} catch (std::runtime_error& re) {
		std::cerr << re.what() << std::endl; 
	}
}

size_t Process::write(const void* data, size_t len)
{
	if(!_w_pid_in.isValid())
		return 0u;
	
	ssize_t size = ::write(_w_pid_in(), data, len);
	if(size == -1)
		_THROW_RUNTIME_ERR("write");

	return static_cast<size_t> (size);
}

void Process::writeExact(const void* data, size_t len)
{
	if(!_w_pid_in.isValid())
		return;

	size_t counter = 0u;
	const char* ch_data = static_cast<const char*> (data);
	while(counter < len) 
		counter += Process::write(ch_data + counter, len - counter);
}

size_t Process::read(void* data, size_t len)
{
	if(!_r_pid_out.isValid())
		return 0u;
		
	ssize_t size = ::read(_r_pid_out(), data, len);
	if (size == -1)
		_THROW_RUNTIME_ERR("read internal failure");
	
	return static_cast<size_t> (size);
}

void Process::readExact(void* data, size_t len)
{
	if(!_r_pid_out.isValid())
		return;

	size_t counter = 0u;
	size_t current = 0u;
	char* ch_data = static_cast<char*> (data);
	while(counter < len) {
		current  = Process::read(ch_data + counter, len - counter);
		if(current == 0)
			_THROW_RUNTIME_ERR("readExact pid failure");
		counter += current;
	}
}

void Process::closeStdin()
{
	_w_pid_in.~Descriptor();
}

void Process::close()
{
	if(kill(_cpid(), SIGINT) == -1) 
		_THROW_RUNTIME_ERR("kill");

	if(waitpid(_cpid(), nullptr, 0) == -1)
		_THROW_RUNTIME_ERR("waitpid");
}