#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include "connection.h"

namespace net {

class BufferedConnection {
 public:
    // TODO: subscribe(read and or write);
    // TODO: unsubscribe(read and or write);
    // TODO: get_read_buf();
    // TODO: get_wtire_buf();
    // TODO: close();
 private:
    // TODO: read_buf_;
    // TODO: write_buf_;
    tcp::Connection connection_;
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
