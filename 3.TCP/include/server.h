#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include "address.h"

namespace tcp
{
    
class Server
{
public:
    Server();
    Server(const Address& addr); // то куда мы встаем и слушаем 
    Server(const Server&  other) = delete;
    Server(Server&& other);
    ~Server();

    void listen(const Address& addr);
    Connection accept();
    
    void close(); // сервер больше не слушает
    void set_timeout(long sec, long usec = 0l) const;

    Server& operator= (const Server&  other) = delete;
    Server& operator= (Server&& other);
private:
    void setSocket();
private:
    Address    s_addr;
    Descripter s_sockfd;
};

} // namespace tcp

#endif // SERVER_h