#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include "process.h"

// using namespace std;

#define _THROW_RUNTIME_ERR(x) throw std::runtime_error(x)
#define _TRY(x) if((x) == -1) _THROW_RUNTIME_ERR(#x)

enum ERROR {
	FD = -1
};

Process::Process(const std::string& path)
{
	_TRY(pipe2(_pipe_out, O_CLOEXEC));
	_TRY(pipe(_pipe_in));
    
	_TRY(_cpid = fork());

	if(_cpid == 0) { /* child process */

		_TRY(dup2(_pipe_in[0], STDIN_FILENO)); /* Child now reads from replaced STDIN_FILENO */
		_TRY(::close(_pipe_in[0]));			   /* Now we do not need the original one*/
		
		_TRY(dup2(_pipe_out[1], STDOUT_FILENO)); /* same */
		_TRY(::close(_pipe_out[1]));
		
		_TRY(execl(path.c_str(), path.c_str(), nullptr)); /* executing.. */		
    } else { /* parrent process */
		_TRY(::close(_pipe_out[1])); /* Parent only reads from _pipe_out */
		_TRY(::close(_pipe_in[0]));	 /* Parent only writes to  _pipe_in */
	}
}

Process::~Process()
{
	if(_pipe_out[0] != ERROR::FD) {
		::close(_pipe_out[0]);
		_pipe_out[0] = ERROR::FD; 
	}
	closeStdin();
	close();
}

size_t Process::write(const void* data, size_t len)
{
	if(_pipe_in[1] != ERROR::FD) {
		long size;
		_TRY(size = ::write(_pipe_in[1], data, len));
		return static_cast<size_t> (size);
	}
	return 0u;
}

void Process::writeExact(const void* data, size_t len)
{
	if(_pipe_in[1] != ERROR::FD) {
		size_t counter = 0u;
		const char* ch_data = static_cast<const char*> (data);
		while(counter++ < len) {
			_TRY(::write(_pipe_in[1], ch_data++, 1));
		}
	}
}

size_t Process::read(void* data, size_t len)
{
	if(_pipe_out[0] != ERROR::FD) {
		long size;
		_TRY(size = ::read(_pipe_out[0], data, len));
		return static_cast<size_t> (size);
	}
	return 0u;
}

void Process::readExact(void* data, size_t len)
{
	if(_pipe_in[0] != ERROR::FD) {
		size_t counter = 0u;
		char* ch_data = static_cast<char*> (data);
		while(counter++ < len) {
			_TRY(::read(_pipe_out[0], ch_data++, 1));
		}
	}
}

void Process::closeStdin()
{
	if(_pipe_in[1] != ERROR::FD) {
		_TRY(::close(_pipe_in[1]));
		_pipe_in[1] = ERROR::FD;
	}
}

void Process::close()
{
	_TRY(kill(_cpid, SIGINT));
	_TRY(waitpid(_cpid, nullptr, 0));
}