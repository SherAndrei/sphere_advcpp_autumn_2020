#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <unistd.h>
#include "error.h"
#include "descripter.h"
#include "connection.h"

using namespace tcp;

static void handle_error(int errnum) 
{ 
    if(errnum == -1)
        throw tcp::Error(std::strerror(errno)); 
}

Connection::Connection(const Address& addr)
    : c_addr(addr)
{
    connect(addr);
}
Connection::Connection(Descripter && fd, Address&& addr)
    :   c_addr(std::move(addr))
    , c_sockfd(std::move(fd))
{}

Connection::Connection(Connection && other)
    :   c_addr(std::move(other.c_addr))
    , c_sockfd(std::move(other.c_sockfd))
{}

void Connection::connect(const Address& addr)
{
    int error; 
    c_sockfd.set_fd(::socket(AF_INET, SOCK_STREAM, 0));
    if(c_sockfd.fd())
        throw SocketError(std::strerror(errno));

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port   = ::htons(addr.port);
    error = ::inet_aton(addr.address.data(), &sock_addr.sin_addr);
    if(error == 0)
        throw AddressError("incorrect address", addr);
    
    error = ::connect(c_sockfd.fd(), reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
    if(error == -1)
        throw AddressError(std::strerror(errno), addr);
}

size_t Connection::write(const void* data, size_t len)
{
	ssize_t size;
	handle_error(size = ::write(c_sockfd.fd(), data, len));

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
	ssize_t size;
	handle_error(size = ::read(c_sockfd.fd(), data, len));
	
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
			throw DescripterError("readExact failure");
		counter += current;
    }
}

void Connection::close() { c_sockfd.close(); }
void Connection::set_timeout(long sec, long usec) const
{
    timeval timeout = { sec, usec };
    handle_error(setsockopt(c_sockfd.fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)));
}

Connection& Connection::operator= (Connection && other)
{
    c_addr   = std::move(other.c_addr);
    other.c_addr.port = 0u;
    c_sockfd = std::move(other.c_sockfd);
    return *this;
}
