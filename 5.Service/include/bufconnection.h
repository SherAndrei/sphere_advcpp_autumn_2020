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

    std::string str() const;

    size_t append(const void* data, size_t len);
    void remove_prefix(size_t size);

    char* data();
    const char* data() const;

    size_t size() const;
    size_t available_size() const;
    size_t max_size() const;

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
    void read(void* data, size_t len) const;

    Buffer& read_buf();
    Buffer& write_buf();
    void close();

 public:
    bool is_open() const;

 public:
    tcp::Descriptor& fd();
    const tcp::Descriptor& fd() const;
    tcp::Address adress() const;

 private:
    friend class Service;
    size_t read_to_buffer();
    size_t write_from_buffer();

 private:
    Buffer read_{512};
    Buffer write_{512};
    tcp::Connection connection_;
    EPoll* p_epoll_;
    OPTION epoll_option_{OPTION::UNKNOW};
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
