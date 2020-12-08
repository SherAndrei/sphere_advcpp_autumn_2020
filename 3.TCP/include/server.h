#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include "iConnectable.h"
#include "nonblock_connection.h"
#include "connection.h"

namespace tcp {

class Server : public IConnectable {
 public:
    explicit Server(const Address& addr);

 public:
    void listen(const Address& addr);
    Connection accept();
    NonBlockConnection accept_non_block();

    void close() override;
    void set_timeout(ssize_t sec, ssize_t usec = 0l) const;
    void set_nonblock() const;
    void set_reuseaddr() const;
};

}  // namespace tcp

#endif  // TCP_SERVER_H
