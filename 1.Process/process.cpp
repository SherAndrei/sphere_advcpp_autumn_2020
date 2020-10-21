#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <exception>
#include "process.h"

static void throw_runtime_err(const std::string& what) { throw std::runtime_error(what); }

Process::Process(const std::string& path, const std::vector<std::string>& params)
{
	open(path, params);
}

static std::vector<char *> argv(std::string& path, std::vector<std::string>& params)
{
	std::vector<char*> result;
	result.reserve(params.size() + 2);

	result.push_back(path.data());

	for(auto& word : params)
		result.push_back(word.data());
	
	result.push_back(nullptr);
	return result;
}

void Process::open(const std::string& path, const std::vector<std::string>& params)
{
	if(isRunning()) {
		throw_runtime_err("Nested processes are forbidden");
	}
	int pipe_in[2], pipe_out[2];
	if(pipe2(pipe_out, O_CLOEXEC) == -1)
		throw_runtime_err("pipe from child");

	if(pipe(pipe_in) == -1) {
		::close(pipe_out[0]);
		::close(pipe_out[1]);
		throw_runtime_err("pipe to child");
	}
	
	Descripter write_to_parent(pipe_out[1]);
	_read_from_child.setID(pipe_out[0]);

	Descripter read_from_parent(pipe_in[0]);
	_write_to_child.setID(pipe_in[1]);

	if((_cpid = fork()) == -1) 
		throw_runtime_err("fork");

	if(_cpid == 0) { /* child process */
		if(::dup2(read_from_parent.id(), STDIN_FILENO) == -1) { // replacing child stdin with pipe
			std::cerr << "dup2 stdin" << std::endl;
			exit(EXIT_FAILURE);
		} 

		// closing pipe_in
		read_from_parent.close();			   
		_write_to_child.close();
		
		if(::dup2(write_to_parent.id(), STDOUT_FILENO) == -1) { // replasing child stdout with pipe
			std::cerr << "dup2 stdout" << std::endl;
			exit(EXIT_FAILURE);
		}

		// closing pipe_out
		write_to_parent.close();
		_read_from_child.close();

		std::string path_copy = path;
		std::vector<std::string> params_copy = params;

		// executing program
		if(execv(path.c_str(), argv(path_copy, params_copy).data()) == -1) {
			std::cerr << "execv" << std::endl;
			exit(EXIT_FAILURE);	
		} 		
    }
}

Process::~Process()
{
	try {
		close();
	} catch (const std::runtime_error& re) {}
}

size_t Process::write(const void* data, size_t len)
{
	ssize_t size = ::write(_write_to_child.id(), data, len);
	if(size == -1)
		throw_runtime_err("write");

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
	ssize_t size = ::read(_read_from_child.id(), data, len);
	if (size == -1)
		throw_runtime_err("read internal failure");
	
	return static_cast<size_t> (size);
}

void Process::readExact(void* data, size_t len)
{
	size_t counter = 0u;
	size_t current = 0u;
	char* ch_data = static_cast<char*> (data);
	while(counter < len) {
		current  = read(ch_data + counter, len - counter);
		if(current == 0)
			throw_runtime_err("readExact pid failure");
		counter += current;
	}
}

void Process::closeStdin()
{
	_write_to_child.close();
}

void Process::close()
{
	_write_to_child.close();
	_read_from_child.close();

	if(kill(_cpid, SIGINT) == -1) 
		throw_runtime_err("kill");

	if(waitpid(_cpid, nullptr, 0) == -1)
		throw_runtime_err("waitpid");
}

bool Process::isRunning() const
{
	return !waitpid(_cpid, nullptr, WNOHANG);;
}