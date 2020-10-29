#ifndef CONNECTION_H
#define CONNECTION_H
#include "descripter.h"
#include <string>

namespace tcp
{
    
class Connection
{
private:
    friend class Server;
    Connection(int client_fd);

public:
    Connection();
    ~Connection();
    Connection(const std::string& addr, uint16_t port); // адрес и порт куда мы хотим подключиться
    
    Connection(const Connection& other) = delete;
    Connection(Connection && other);
    
    void connect(const std::string& addr, uint16_t port);
    void close();
    
    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);
 
    void set_timeout(long sec, long usec = 0l) const;

    Connection& operator= (const Connection &  other) = delete;
    Connection& operator= (Connection && other);

private:
    void setSocket();
private:
    Descripter c_sockfd;
};

} // namespace tcp

#endif // CONNECTION_H