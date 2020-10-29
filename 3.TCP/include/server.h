#ifndef SERVER_H
#define SERVER_H
#include <vector>
#include "connection.h"

namespace tcp
{
    
class Server
{
public:
    Server();
    Server(const std::string&  addr, uint16_t port); // то куда мы встаем и слушаем 
    Server(const Server&  other) = delete;
    Server(Server&& other);
    ~Server();
    
    void listen(const std::string&  addr, uint16_t port);
    Connection accept();
    
    void close(); // сервер больше не слушает
    void set_timeout(long sec, long usec = 0l) const;

    Server& operator= (const Server&  other) = delete;
    Server& operator= (Server&& other);
private:
    void setSocket();
private:
    Descripter s_sockfd;
};

} // namespace tcp

#endif // SERVER_h