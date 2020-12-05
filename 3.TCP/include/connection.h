#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include "iConnectable.h"

namespace tcp {

class Connection : public IConnectable {
 private:
    friend class Server;
    Connection(Descriptor && fd, const Address& addr);

 public:
    explicit Connection(const Address& addr);

 public:
    void connect(const Address&);
    void close() override;

    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);

    void set_timeout(ssize_t sec, ssize_t usec = 0l) const;
    void set_nonblock() const;
};

}  // namespace tcp

#endif  // TCP_CONNECTION_H
