#ifndef TCP_NONBLOCK_CONNECTION_H
#define TCP_NONBLOCK_CONNECTION_H
#include <string>
#include "iConnection.h"
#include "socket.h"

namespace tcp {

class NonBlockConnection : public IConnection {
 private:
    friend class Server;
    NonBlockConnection(Socket&& socket, const Address& addr);

 public:
    using IConnection::IConnection;

 public:
    void close() override;

    virtual size_t write(const std::string& data);
    virtual size_t read(std::string& data);
};

}  // namespace tcp

#endif  // TCP_NONBLOCK_CONNECTION_H
