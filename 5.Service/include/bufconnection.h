#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "connection.h"
#include "option.h"

namespace net {

class Service;

class BufferedConnection {
 public:
    BufferedConnection() = default;
    explicit BufferedConnection(tcp::Connection && other);

    BufferedConnection& operator= (const BufferedConnection&  other) = delete;
    BufferedConnection(const BufferedConnection& other)              = delete;

    BufferedConnection(BufferedConnection && other)             = default;
    BufferedConnection& operator= (BufferedConnection && other) = default;

    virtual ~BufferedConnection() = default;

 public:
    virtual void subscribe(OPTION opt);
    virtual void unsubscribe(OPTION opt);
    void write(const std::string& data);
    void read(std::string& data);
    virtual void close();

 public:
    std::string& read_buf();
    std::string& write_buf();

    tcp::Address address() const;

    tcp::Descriptor& socket();
    const tcp::Descriptor& socket() const;

 private:
    friend class Service;

 protected:
    size_t read_to_buffer();
    size_t write_from_buffer();

 protected:
    std::string read_;
    std::string write_;
    tcp::Connection connection_;
    OPTION epoll_option_{OPTION::UNKNOWN};
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
