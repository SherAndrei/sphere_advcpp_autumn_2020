#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include "descriptor.h"
#include "address.h"

namespace tcp {

class Connection {
 private:
    friend class Server;
    Connection(Descriptor && fd, const Address& addr);

 public:
    Connection()  = default;
    explicit Connection(const Address& addr);

    Connection& operator= (const Connection &  other) = delete;
    Connection(const Connection& other) = delete;

    Connection(Connection && other);
    Connection& operator= (Connection && other);

    ~Connection() = default;

 public:
    void connect(const Address&);
    void close();

    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);

    void set_timeout(ssize_t sec, ssize_t usec = 0l) const;
    void set_nonblock() const;

 public:
    Descriptor& fd();

 private:
    Address    c_addr;
    Descriptor c_sock;
};

}  // namespace tcp

#endif  // TCP_CONNECTION_H
