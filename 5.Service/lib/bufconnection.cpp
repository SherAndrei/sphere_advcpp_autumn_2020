#include "bufconnection.h"
#include <algorithm>

namespace net {

Buffer::Buffer(size_t size)
    : _max(size) {}

std::string Buffer::str() const {
    return buf_;
}
size_t Buffer::append(const void* data, size_t len) {
    const char* ch_d = static_cast<const char*>(data);
    const std::string to_app = {ch_d, ch_d + std::min(available_size(), len)};
    buf_ += to_app;
    return to_app.size();
}

void Buffer::remove_prefix(size_t len) {
    if (len <= size())
        buf_.erase(0, len);
}

char* Buffer::data() {
    return buf_.data();
}
const char* Buffer::data() const {
    return buf_.data();
}
size_t Buffer::size() const {
    return buf_.size();
}
size_t Buffer::available_size() const {
    return max_size() - size();
}
size_t Buffer::max_size() const {
    return _max;
}
void Buffer::clear() {
    buf_.clear();
}
bool Buffer::empty() const {
    return buf_.empty();
}

BufferedConnection::BufferedConnection(tcp::Connection && other, EPoll* p_epoll)
    : connection_(std::move(other)), p_epoll_(p_epoll) {}

void BufferedConnection::write(const void* data, size_t len) {
    write_.append(data, len);
}
void BufferedConnection::read(void* data, size_t len) const {
    char* ch_d = static_cast<char*>(data);
    std::copy(read_.data(), read_.data() + len, ch_d);
}

void BufferedConnection::subscribe(OPTION opt) {
    epoll_option_ = epoll_option_ + opt;
    p_epoll_->mod(connection_.fd(), epoll_option_);
}

void BufferedConnection::unsubscribe(OPTION opt) {
    epoll_option_ = epoll_option_ - opt;
    p_epoll_->mod(connection_.fd(), epoll_option_);
}

Buffer& BufferedConnection::read_buf() {
    return read_;
}

Buffer& BufferedConnection::write_buf() {
    return write_;
}

void BufferedConnection::close() {
    p_epoll_->del(connection_.fd());
    epoll_option_ = OPTION::UNKNOW;
    connection_.close();
}
tcp::Descriptor& BufferedConnection::fd() {
    return connection_.fd();
}
const tcp::Descriptor& BufferedConnection::fd() const {
    return connection_.fd();
}

tcp::Address BufferedConnection::adress() const {
    return connection_.address();
}

size_t BufferedConnection::read_to_buffer() {
    std::string buf(512, '\0');
    size_t size;
    size = connection_.read(buf.data(), buf.size());
    return read_.append(buf.data(), size);
}

size_t BufferedConnection::write_from_buffer() {
    size_t size;
    size = connection_.write(write_.data(), write_.size());
    write_.remove_prefix(size);
    return size;
}


}  // namespace net
