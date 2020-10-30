#ifndef CONNECTION_H
#define CONNECTION_H
#include "descripter.h"
#include "address.h"

namespace tcp
{
    
class Connection
{
private:
    friend class Server;
    Connection(Descripter && fd, Address&& addr);

public:
    Connection()  = default;
    Connection(const Address& addr); // адрес и порт куда мы хотим подключиться
    Connection(const Connection& other) = delete;
    Connection(Connection && other);
    ~Connection() = default;
    
    void connect(const Address& addr);
    void close();
    
    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);
 
    void set_timeout(long sec, long usec = 0l) const;

    Connection& operator= (const Connection &  other) = delete;
    Connection& operator= (Connection && other);

private:
    Address    c_addr;
    Descripter c_sockfd;
};

} // namespace tcp

#endif // CONNECTION_H