#include <algorithm>
#include "bufconnection.h"

namespace net {

BufferedConnection::BufferedConnection(tcp::Connection && other)
    : connection_(std::move(other)) {}

void BufferedConnection::write(const std::string& data) {
    write_.append(data);
}
void BufferedConnection::read(std::string& data) {
    data = read_;
}

static constexpr size_t BUF_SIZE = 128;

size_t BufferedConnection::read_to_buffer() {
    std::string buf(BUF_SIZE, '\0');
    size_t size = connection_.read(buf.data(), BUF_SIZE);
    buf.resize(size);
    read_ += buf;
    return size;
}

size_t BufferedConnection::write_from_buffer() {
    size_t size;
    size = connection_.write(write_.data(), write_.size());
    write_.erase(0, size);
    return size;
}

void BufferedConnection::subscribe(OPTION opt) {
    epoll_option_ = epoll_option_ + opt + net::OPTION::CLOSE;
}

void BufferedConnection::unsubscribe(OPTION opt) {
    epoll_option_ = epoll_option_ - opt  + net::OPTION::CLOSE;
}

void BufferedConnection::close() {
    epoll_option_ = OPTION::UNKNOWN;
    connection_.close();
}

std::string& BufferedConnection::read_buf() {
    return read_;
}

std::string& BufferedConnection::write_buf() {
    return write_;
}

tcp::Descriptor& BufferedConnection::socket() {
    return connection_.socket();
}

const tcp::Descriptor& BufferedConnection::socket() const {
    return connection_.socket();
}

tcp::Address BufferedConnection::address() const {
    return connection_.address();
}

}  // namespace net
