#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"

namespace net {

struct Buffer {
    std::string data;
};

class BufferedConnection {
 public:
    BufferedConnection()  = default;
    explicit BufferedConnection(tcp::Connection && other);

    BufferedConnection& operator= (const BufferedConnection&  other) = delete;
    BufferedConnection(const BufferedConnection& other)              = delete;

    BufferedConnection(BufferedConnection && other)             = default;
    BufferedConnection& operator= (BufferedConnection && other) = default;

    ~BufferedConnection() = default;

 public:
    // TODO: subscribe(read and or write);
    // TODO: unsubscribe(read and or write);
    void write_from_buf();
    void read_to_buf();

    Buffer read_buf();
    Buffer write_buf();
    void close();

 public:
    tcp::Descriptor& fd();
    tcp::Connection& connection();

 private:
    Buffer read_;
    Buffer write_;
    tcp::Connection connection_;
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
