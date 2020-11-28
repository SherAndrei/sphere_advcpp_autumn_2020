#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"
#include "epoll.h"

namespace net {

class Service;

class BufferedConnection : public tcp::Connection {
 public:
    BufferedConnection() = default;
    explicit BufferedConnection(tcp::Connection && other);

    BufferedConnection& operator= (const BufferedConnection&  other) = delete;
    BufferedConnection(const BufferedConnection& other)              = delete;

    BufferedConnection(BufferedConnection && other)             = default;
    BufferedConnection& operator= (BufferedConnection && other) = default;

    virtual ~BufferedConnection() = default;

 public:
    void subscribe(OPTION opt);
    void unsubscribe(OPTION opt);
    void write(const std::string& data);
    void read(std::string& data);
    virtual void close();

    std::string& read_buf();
    std::string& write_buf();

 private:
    friend class Service;

 protected:
    size_t read_to_buffer();
    size_t write_from_buffer();

 protected:
    std::string read_;
    std::string write_;
    OPTION epoll_option_{OPTION::UNKNOW};
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
