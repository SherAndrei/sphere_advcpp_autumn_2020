#ifndef NET_BUFFERED_CONNECTION_H
#define NET_BUFFERED_CONNECTION_H
#include <string>
#include "nonblock_connection.h"
#include "option.h"

namespace net {

class Service;

class BufferedConnection : public tcp::NonBlockConnection {
 public:
    explicit BufferedConnection(tcp::NonBlockConnection && other);

 public:
    virtual void subscribe(OPTION opt);
    virtual void unsubscribe(OPTION opt);
    size_t write(const std::string& data) override;
    size_t read(std::string& data) override;
    virtual void close();

 public:
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
    OPTION epoll_option_{OPTION::UNKNOWN};
};

}  // namespace net

#endif  // NET_BUFFERED_CONNECTION_H
