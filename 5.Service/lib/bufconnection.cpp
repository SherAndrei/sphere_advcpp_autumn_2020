#include "bufconnection.h"
#include <algorithm>

namespace net {

BufferedConnection::BufferedConnection(tcp::NonBlockConnection && other)
    : tcp::NonBlockConnection(std::move(other)) {}

size_t BufferedConnection::write(const std::string& data) {
    write_.append(data);
    return data.length();
}

size_t BufferedConnection::read(std::string& data) {
    data = read_;
    return data.length();
}

static constexpr size_t BUF_SIZE = 128;

size_t BufferedConnection::read_to_buffer() {
    std::string buf(BUF_SIZE, '\0');
    size_t size = tcp::NonBlockConnection::read(buf);
    buf.resize(size);
    read_ += buf;
    return size;
}

size_t BufferedConnection::write_from_buffer() {
    size_t size;
    size = tcp::NonBlockConnection::write(write_);
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
    tcp::NonBlockConnection::close();
}

std::string& BufferedConnection::read_buf() {
    return read_;
}

std::string& BufferedConnection::write_buf() {
    return write_;
}

}  // namespace net
