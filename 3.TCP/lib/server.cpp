#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <unistd.h>
#include "descripter.h"
#include "connection.h"
#include "server.h"
#include "error.h"

static void throw_error(const std::string& what) 
    { throw tcp::Error(what); }

using namespace tcp;

Server::Server() { setSocket(); }
Server::Server(const std::string&  addr, uint16_t port)
{
    setSocket();
    listen(addr, port);
}
Server::~Server() { close(); }

Server::Server(Server&& other) 
    : s_sockfd(std::move(other.s_sockfd))
{ }

void Server::listen(const std::string&  addr, const uint16_t port)
{
    int error;
    if(!s_sockfd.valid())
        setSocket();

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(port);
    error = ::inet_aton(addr.data(), &sock_addr.sin_addr);
    if(error == 0)
        throw tcp::AddressError("Incorrect address!", addr, port);

    // привязываем дескриптор к сокету
    error = ::bind(s_sockfd.fd(), reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
    if(error == -1)
        throw tcp::AddressError(std::strerror(errno), addr, port);

    error = ::listen(s_sockfd.fd(), SOMAXCONN);
    if(error == -1)
        throw tcp::AddressError(std::strerror(errno), addr, port);
}
Connection Server::accept()
{
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client = ::accept(s_sockfd.fd(),reinterpret_cast<sockaddr*>(&peer_addr), &s);
    if(client == -1)
        throw_error(std::strerror(errno));

    return Connection{client};
}

void Server::close()
{
    s_sockfd.close();
} 

void Server::set_timeout(long sec, long usec) const
{
    timeval timeout = { sec, usec };
    if(setsockopt(s_sockfd.fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
        throw_error(std::strerror(errno));
   
}

void Server::setSocket()
{
    s_sockfd.set_fd(::socket(AF_INET, SOCK_STREAM, 0));
    if(!s_sockfd.valid())
        throw_error(std::strerror(errno));
}

Server& Server::operator= (Server&& other)
{
    s_sockfd = std::move(other.s_sockfd);
    return *this;
}
