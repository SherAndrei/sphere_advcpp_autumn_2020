#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"
#include "epoll.h"

namespace net {

class Service;

class Buffer {
    size_t _max;
    std::string buf_;
 public:
    explicit Buffer(size_t size);

    void* data();
    const void* data() const;
    size_t size()     const;
    size_t max_size() const;
    void fill(const void* data, size_t len);
    void clear();
    bool empty() const;
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

    Buffer& read_buf();
    Buffer& write_buf();
    void close();

 public:
    tcp::Descriptor& fd();
    tcp::Address adress() const;

 private:
    Buffer read_  = Buffer{512};
    Buffer write_ = Buffer{512};
    tcp::Connection connection_;
    EPoll* p_epoll_;
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
