#ifndef CONNECTION_H
#define CONNECTION_H
#include "descripter.h"
#include <string>

class Connection
{
    friend class Server;
public:
    Connection();
    ~Connection();
    Connection(const std::string& addr, uint16_t port); // адрес и порт куда мы хотим подключиться
    
    void connect(const std::string& addr, uint16_t port);
    void close();
    
    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);
 
    void set_timeout(long sec, long usec = 0l) const;

    Connection(Connection& other) = delete;
    // TODO: operator(move)();
private:
    Connection(int client_fd);

private:
    void setSocket();
private:
    Descripter c_sockfd;
};

#endif // CONNECTION_H