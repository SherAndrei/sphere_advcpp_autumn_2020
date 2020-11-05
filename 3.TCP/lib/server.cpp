#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/ip.h>
#include <unistd.h>
#include "descripter.h"
#include "connection.h"
#include "server.h"
#include "address.h"
#include "error.h"

static void handle_error(int errnum) 
{ 
    if(errnum == -1)
        throw tcp::Error(std::strerror(errno)); 
}

using namespace tcp;

Server::Server(const Address& addr)
    : s_addr(addr)
{
    listen(addr);
}
Server::Server(Server&& other) 
    :   s_addr(std::move(other.s_addr))
    , s_sockfd(std::move(other.s_sockfd))
{ }

void Server::listen(const Address& addr)
{
    int error;
    // Для строгой гарантии исключений
    Descripter temp(::socket(AF_INET, SOCK_STREAM, 0));
    if(temp.fd())
        throw SocketError(std::strerror(errno));

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(addr.port());
    error = ::inet_aton(addr.address().data(), &sock_addr.sin_addr);
    if(error == 0)
        throw tcp::AddressError("Incorrect address!", addr);

    // привязываем дескриптор к сокету
    error = ::bind(temp.fd(), reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
    if(error == -1)
        throw tcp::AddressError(std::strerror(errno), addr);

    error = ::listen(temp.fd(), SOMAXCONN);
    if(error == -1)
        throw tcp::AddressError(std::strerror(errno), addr);
    
    s_sockfd = std::move(temp); 
}
Connection Server::accept()
{
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client;
    handle_error(client = ::accept(s_sockfd.fd(),reinterpret_cast<sockaddr*>(&peer_addr), &s));

    return Connection{ Descripter{client}, 
                       Address{ ::inet_ntoa(peer_addr.sin_addr), peer_addr.sin_port } };
}

void Server::close()
{
    s_sockfd.close();
} 

void Server::set_timeout(long sec, long usec) const
{
    timeval timeout = { sec, usec };
    handle_error(setsockopt(s_sockfd.fd(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)));
    handle_error(setsockopt(s_sockfd.fd(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)));
}

Server& Server::operator= (Server&& other)
{
    this->s_addr   = std::move(other.s_addr);
    this->s_sockfd = std::move(other.s_sockfd);
    return *this;
}
