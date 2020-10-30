#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include "address.h"

namespace tcp
{
    
class Server
{
public:
    Server() = default;
    Server(const Address& addr);
    Server(const Server&  other) = delete;
    Server(Server&& other);
    ~Server() = default;

    Server& operator= (const Server&  other) = delete;
    Server& operator= (Server&& other);

    void listen(const Address& addr);
    Connection accept();
    
    void close(); // сервер больше не слушает
    void set_timeout(long sec, long usec = 0l) const;

private:
    Address    s_addr;
    Descripter s_sockfd;
};

} // namespace tcp

#endif // SERVER_h