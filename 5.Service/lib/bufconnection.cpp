#include "bufconnection.h"
#include <algorithm>

net::Buffer::Buffer(size_t size)
    : _max(size), buf_() {}

void net::Buffer::load(const void* source, size_t len) {
    const char* ch_data = static_cast<const char*>(source);
    buf_.assign(ch_data, ch_data + std::min(_max, len));
}

void net::Buffer::unload(void* dest, size_t len) const {
    char* ch_dest = static_cast<char*>(dest);
    std::copy(buf_.data(), buf_.data() + len, ch_dest);
}

void* net::Buffer::data() {
    return buf_.data();
}

const void* net::Buffer::data() const {
    return buf_.data();
}
size_t net::Buffer::size() const {
    return buf_.size();
}

void*  net::Buffer::remaining_space() {
    return buf_.data() + buf_.length();
}
size_t net::Buffer::remaining_size() const {
    return _max - buf_.length();
}

void net::Buffer::clear() {
    buf_.clear();
}

size_t net::Buffer::max_size() const {
    return _max;
}

bool net::Buffer::empty() const {
    return buf_.empty();
}

net::BufferedConnection::BufferedConnection(tcp::Connection && other, EPoll* p_epoll)
    : connection_(std::move(other)), p_epoll_(p_epoll) {}

void net::BufferedConnection::write(const void* data, size_t len) {
    write_.load(data, len);
}
void net::BufferedConnection::read(void* data, size_t len) const {
    read_.unload(data, len);
}

static net::OPTION add(net::OPTION lhs, net::OPTION rhs) {
    unsigned i_lhs = static_cast<unsigned>(lhs);
    unsigned i_rhs = static_cast<unsigned>(rhs);
    i_lhs |= i_rhs;
    return static_cast<net::OPTION>(i_lhs);
}

void net::BufferedConnection::subscribe(OPTION opt) {
    p_epoll_->set_option(add(p_epoll_->option(), opt));
    p_epoll_->add(connection_.fd());
}

static net::OPTION remove(net::OPTION lhs, net::OPTION rhs) {
    unsigned i_lhs = static_cast<unsigned>(lhs);
    unsigned i_rhs = static_cast<unsigned>(rhs);
    i_lhs &= ~i_rhs;
    return static_cast<net::OPTION>(i_lhs);
}

void net::BufferedConnection::unsubscribe(OPTION opt) {
    p_epoll_->set_option(remove(p_epoll_->option(), opt));
    p_epoll_->mod(connection_.fd());
}

net::Buffer& net::BufferedConnection::read_buf() {
    return read_;
}

net::Buffer& net::BufferedConnection::write_buf() {
    return write_;
}

void net::BufferedConnection::close() {
    p_epoll_->del(connection_.fd());
    connection_.close();
}

tcp::Descriptor& net::BufferedConnection::fd() {
    return connection_.fd();
}

tcp::Address net::BufferedConnection::adress() const {
    return connection_.address();
}
