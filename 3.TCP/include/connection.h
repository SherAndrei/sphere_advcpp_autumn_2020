#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include "BaseConnection.h"

namespace tcp {

class Connection : public BaseConnection {
 private:
    friend class Server;
    Connection(Socket&& socket, const Address& addr);

 public:
    explicit Connection(const Address& addr);

 public:
    void connect(const Address&);
    void close() override;

    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);
};

}  // namespace tcp

#endif  // TCP_CONNECTION_H
