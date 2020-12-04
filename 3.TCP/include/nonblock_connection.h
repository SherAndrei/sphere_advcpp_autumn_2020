#ifndef TCP_NONBLOCK_CONNECTION_H
#define TCP_NONBLOCK_CONNECTION_H
#include <string>
#include "iConnectable.h"

namespace tcp {

class NonBlockConnection : public IConnectable {
 private:
    friend class Server;
    NonBlockConnection(Descriptor&& opened_fd, const Address& addr);

 public:
    using IConnectable::IConnectable;

 public:
    void close() override;

    virtual size_t write(const std::string& data);
    virtual size_t read(std::string& data);
};

}  // namespace tcp

#endif  // TCP_NONBLOCK_CONNECTION_H
