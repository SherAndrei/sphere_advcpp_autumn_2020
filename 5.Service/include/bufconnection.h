#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"
#include "epoll.h"

namespace net {

class Service;

class BufferedConnection {
 public:
    BufferedConnection() = default;
    BufferedConnection(tcp::Connection && other, EPoll* p_epoll);

    BufferedConnection& operator= (const BufferedConnection&  other) = delete;
    BufferedConnection(const BufferedConnection& other)              = delete;

    BufferedConnection(BufferedConnection && other)             = default;
    BufferedConnection& operator= (BufferedConnection && other) = default;

    ~BufferedConnection() = default;

 public:
    void subscribe(OPTION opt);
    void unsubscribe(OPTION opt);
    void write(const std::string& data);
    void read(std::string& data);

    std::string& read_buf();
    std::string& write_buf();
    void close();

 public:
    tcp::Descriptor& fd();
    const tcp::Descriptor& fd() const;
    tcp::Address adress() const;

 private:
    friend class Service;
    size_t read_to_buffer();
    size_t write_from_buffer();

 private:
    std::string read_;
    std::string write_;
    tcp::Connection connection_;
    EPoll* p_epoll_;
    OPTION epoll_option_{OPTION::UNKNOW};
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
