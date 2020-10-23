#ifndef SERVER_H
#define SERVER_H
#include <vector>
#include "connection.h"

// TCP
class Server
{
public:
    Server();
    Server(const std::string&  addr, uint16_t port); // то куда мы встаем и слушаем 
    void listen(const std::string&  addr, uint16_t port);
    Connection accept();
    void close(); // сервер больше не слушает
    //TODO: operator move
    void set_timeout(long sec, long usec = 0l) const;
private:
    void setSocket();
private:
    Descripter s_sockfd;
};

#endif // SERVER_h