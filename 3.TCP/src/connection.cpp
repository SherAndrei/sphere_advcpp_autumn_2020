#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include "descripter.h"
#include "connection.h"
#include <netinet/ip.h>
#include <unistd.h>

static void throw_runtime_err(const std::string& what) 
    { throw std::runtime_error(what); }

Connection::Connection()
{
    setSocket();
}
Connection::~Connection()
{
    c_sockfd.close();
}
Connection::Connection(const std::string& addr, uint16_t port)
{
    setSocket();
    connect(addr, port);
}
Connection::Connection(int client_fd)
{
    c_sockfd.setID(client_fd);
}
void Connection::connect(const std::string& addr, uint16_t port)
{
    int error; 
    if(!c_sockfd.isValid()) {
        setSocket();
    }
    // TODO: проверять что соединение уже установлено

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port   = ::htons(port);
    error = ::inet_aton(addr.data(), &sock_addr.sin_addr);
    if(error == 0)
        throw_runtime_err("Incorrect address!");
    
    error = ::connect(c_sockfd.id(), reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
    if(error == -1)
        throw_runtime_err(std::strerror(errno));
}

size_t Connection::write(const void* data, size_t len)
{
	ssize_t size = ::write(c_sockfd.id(), data, len);
	if(size == -1)
		throw_runtime_err(std::strerror(errno));

	return static_cast<size_t> (size);
}

void   Connection::writeExact(const void* data, size_t len)
{
	size_t counter = 0u;
	const char* ch_data = static_cast<const char*> (data);
	while(counter < len) 
		counter += write(ch_data + counter, len - counter);
}
size_t Connection::read(void* data, size_t len)
{
	ssize_t size = ::read(c_sockfd.id(), data, len);
	if (size == -1)
		throw_runtime_err(std::strerror(errno));
	
	return static_cast<size_t> (size);
}
void   Connection::readExact(void* data, size_t len)
{
	size_t counter = 0u;
	size_t current = 0u;
	char* ch_data = static_cast<char*> (data);
	while(counter < len) {
		current  = read(ch_data + counter, len - counter);
		if(current == 0u)
			throw_runtime_err("readExact pid failure");
		counter += current;
    }
}

void Connection::setSocket()
{
    c_sockfd.setID(::socket(AF_INET, SOCK_STREAM, 0));
    if(!c_sockfd.isValid())
        throw_runtime_err(std::strerror(errno));
}

void Connection::close()
{
    c_sockfd.close();
}
void Connection::set_timeout(long sec, long usec) const
{
    timeval timeout = { sec, usec };
    if(setsockopt(c_sockfd.id(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
        throw_runtime_err(std::strerror(errno));
}
