#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"
#include "epoll.h"

namespace net {

struct Buffer {
    std::string data;
};

class BufferedConnection {
 public:
    BufferedConnection()  = default;
    BufferedConnection(tcp::Connection && other, EPoll* p_epoll);

    BufferedConnection& operator= (const BufferedConnection&  other) = delete;
    BufferedConnection(const BufferedConnection& other)              = delete;

    BufferedConnection(BufferedConnection && other)             = default;
    BufferedConnection& operator= (BufferedConnection && other) = default;

    ~BufferedConnection() = default;

 public:
    void subscribe(OPTION opt);
    void unsubscribe(OPTION opt);
    void write(const void* data, size_t len);
    void read(void* data, size_t len);

    Buffer  read_buf();
    Buffer& write_buf();
    void close();

 public:
    tcp::Descriptor& fd();

 private:
    Buffer read_;
    Buffer write_;
    tcp::Connection connection_;
    EPoll* p_epoll_;
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
