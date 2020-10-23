#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include <netinet/ip.h>
#include <unistd.h>
#include "descripter.h"
#include "connection.h"
#include "server.h"

static void throw_runtime_err(const std::string& what) 
    { throw std::runtime_error(what); }

Server::Server()
{
    setSocket();
}
Server::Server(const std::string&  addr, uint16_t port)
{
    setSocket();
    listen(addr, port);
}
void Server::listen(const std::string&  addr, uint16_t port)
{
    int error;
    if(!s_sockfd.isValid())
        setSocket();

    sockaddr_in sock_addr{};
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ::htons(port);
    error = ::inet_aton(addr.data(), &sock_addr.sin_addr);
    if(error == 0)
        throw_runtime_err("Incorrect address!");

    // привязываем дескриптор к сокету
    error = ::bind(s_sockfd.id(), reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
    if(error == -1)
        throw_runtime_err(std::strerror(errno));

    error = ::listen(s_sockfd.id(), SOMAXCONN);
    if(error == -1)
        throw_runtime_err(std::strerror(errno));
}
Connection Server::accept()
{
    sockaddr_in peer_addr{};
    socklen_t s = sizeof(peer_addr);
    int client = ::accept(s_sockfd.id(),reinterpret_cast<sockaddr*>(&peer_addr), &s);
    if(client == -1)
        throw_runtime_err(std::strerror(errno));

    return Connection{client};
}
void Server::close()
{
    s_sockfd.close();
} 
//TODO: operator move
void Server::set_timeout(long sec, long usec) const
{
    timeval timeout = { sec, usec };
    if(setsockopt(s_sockfd.id(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
        throw_runtime_err(std::strerror(errno));
   
}
void Server::setSocket()
{
    s_sockfd.setID(::socket(AF_INET, SOCK_STREAM, 0));
    if(!s_sockfd.isValid())
        throw_runtime_err(std::strerror(errno));
}