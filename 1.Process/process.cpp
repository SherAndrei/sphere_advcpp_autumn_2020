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
	
	Descripter w_pid_out(pipe_out[1]);
	_r_pid_out.setID(pipe_out[0]);

	Descripter r_pid_in(pipe_in[0]);
	_w_pid_in.setID(pipe_in[1]);

	if((_cpid = fork()) == -1) 
		_THROW_RUNTIME_ERR("fork");

	if(_cpid == 0) { /* child process */
		if(::dup2(r_pid_in.id(), STDIN_FILENO) == -1) {
			std::cerr << "dup2 stdin" << std::endl;
			exit(EXIT_FAILURE);
		} /* Child now reads from replaced STDIN_FILENO */
		r_pid_in.close();			   /* Now we do not need the original one*/
		
		if(::dup2(w_pid_out.id(), STDOUT_FILENO) == -1) { /* same */
			std::cerr << "dup2 stdout" << std::endl;
			exit(EXIT_FAILURE);
		} 
		w_pid_out.close();
		
		if(execl(path.c_str(), path.c_str(), nullptr) == -1) { /* executing.. */
			std::cerr << "execl" << std::endl;
			exit(EXIT_FAILURE);	
		} 		
    }
	// pipe_in[0] и pipe_out[1] закрываются через деструкторы
}

Process::~Process()
{
	_r_pid_out.close();
	_w_pid_in.close();
	try {
		close();
	} catch (std::runtime_error& re) {
		std::cerr << re.what() << std::endl; 
	}
}

size_t Process::write(const void* data, size_t len)
{
	ssize_t size = ::write(_w_pid_in.id(), data, len);
	if(size == -1)
		_THROW_RUNTIME_ERR("write");

	return static_cast<size_t> (size);
}

void Process::writeExact(const void* data, size_t len)
{
	size_t counter = 0u;
	const char* ch_data = static_cast<const char*> (data);
	while(counter < len) 
		counter += write(ch_data + counter, len - counter);
}

size_t Process::read(void* data, size_t len)
{
	ssize_t size = ::read(_r_pid_out.id(), data, len);
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
		current  = read(ch_data + counter, len - counter);
		if(current == 0)
			_THROW_RUNTIME_ERR("readExact pid failure");
		counter += current;
	}
}

void Process::closeStdin()
{
	_w_pid_in.close();
}

void Process::close()
{
	if(kill(_cpid, SIGINT) == -1) 
		_THROW_RUNTIME_ERR("kill");

	if(waitpid(_cpid, nullptr, 0) == -1)
		_THROW_RUNTIME_ERR("waitpid");
}