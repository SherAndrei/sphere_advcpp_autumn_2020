#include "bufconnection.h"
#include <algorithm>

namespace net {

Buffer::Buffer(size_t size)
    : _max(size), buf_() {}

void Buffer::load(const void* source, size_t len) {
    const char* ch_data = static_cast<const char*>(source);
    buf_.assign(ch_data, ch_data + std::min(_max, len));
}

void Buffer::unload(void* dest, size_t len) const {
    char* ch_dest = static_cast<char*>(dest);
    std::copy(buf_.data(), buf_.data() + len, ch_dest);
}

void* Buffer::data() {
    return buf_.data();
}

const void* Buffer::data() const {
    return buf_.data();
}
size_t Buffer::size() const {
    return buf_.size();
}

void*  Buffer::remaining_space() {
    return buf_.data() + buf_.length();
}
size_t Buffer::remaining_size() const {
    return _max - buf_.length();
}

void Buffer::clear() {
    buf_.clear();
}

size_t Buffer::max_size() const {
    return _max;
}

bool Buffer::empty() const {
    return buf_.empty();
}

BufferedConnection::BufferedConnection(tcp::Connection && other, EPoll* p_epoll)
    : connection_(std::move(other)), p_epoll_(p_epoll) {}

void BufferedConnection::write(const void* data, size_t len) {
    write_.load(data, len);
}
void BufferedConnection::read(void* data, size_t len) const {
    read_.unload(data, len);
}
static OPTION add(OPTION lhs, OPTION rhs) {
    unsigned i_lhs = static_cast<unsigned>(lhs);
    unsigned i_rhs = static_cast<unsigned>(rhs);
    i_lhs |= i_rhs;
    return static_cast<OPTION>(i_lhs);
}
static OPTION remove(OPTION lhs, OPTION rhs) {
    unsigned i_lhs = static_cast<unsigned>(lhs);
    unsigned i_rhs = static_cast<unsigned>(rhs);
    i_lhs &= ~i_rhs;
    return static_cast<OPTION>(i_lhs);
}
void BufferedConnection::subscribe(OPTION opt) {
    p_epoll_->mod(connection_.fd(), add(epoll_option_, opt));
}

void BufferedConnection::unsubscribe(OPTION opt) {
    p_epoll_->mod(connection_.fd(), remove(epoll_option_, opt));
}

Buffer& BufferedConnection::read_buf() {
    return read_;
}

Buffer& BufferedConnection::write_buf() {
    return write_;
}

void BufferedConnection::close() {
    p_epoll_->del(connection_.fd());
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

}  // namespace net
