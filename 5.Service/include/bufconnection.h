#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"

namespace net {

struct Buffer {
    std::string buff;
};

class BufferedConnection {
 public:
    // TODO: subscribe(read and or write);
    // TODO: unsubscribe(read and or write);
    // TODO: get_read_buf();
    // TODO: get_wtire_buf();
    void close();
 private:
    Buffer read_;
    Buffer write_;
    tcp::Connection connection_;
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
