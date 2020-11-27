#include "bufconnection.h"
#include <algorithm>

namespace net {

BufferedConnection::BufferedConnection(tcp::Connection && other, EPoll* p_epoll)
    : Connection(std::move(other))
    , p_epoll_(p_epoll) {}

void BufferedConnection::write(const std::string& data) {
    write_.append(data);
}
void BufferedConnection::read(std::string& data) {
    data = read_;
}

static constexpr size_t BUF_SIZE = 128;

size_t BufferedConnection::read_to_buffer() {
    std::string buf(BUF_SIZE, '\0');
    size_t size = Connection::read(buf.data(), BUF_SIZE);
    buf.resize(size);
    read_ += buf;
    return size;
}

size_t BufferedConnection::write_from_buffer() {
    size_t size;
    size = Connection::write(write_.data(), write_.size());
    write_.erase(0, size);
    return size;
}

void BufferedConnection::subscribe(OPTION opt) {
    epoll_option_ = epoll_option_ + opt;
    p_epoll_->mod(c_sock, epoll_option_);
}

void BufferedConnection::unsubscribe(OPTION opt) {
    epoll_option_ = epoll_option_ - opt;
    p_epoll_->mod(c_sock, epoll_option_);
}

std::string& BufferedConnection::read_buf() {
    return read_;
}

std::string& BufferedConnection::write_buf() {
    return write_;
}

void BufferedConnection::close() {
    epoll_option_ = OPTION::UNKNOW;
    Connection::close();
}

}  // namespace net
