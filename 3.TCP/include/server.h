#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include "nonblock_connection.h"
#include "connection.h"

namespace tcp {

class Server : public IConnection {
 public:
    explicit Server(const Address& addr);

 public:
    void listen(const Address& addr);
    Connection accept();
    NonBlockConnection accept_non_block();

    void close() override;
};

}  // namespace tcp

#endif  // TCP_SERVER_H
