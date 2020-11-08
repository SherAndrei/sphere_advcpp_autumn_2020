#include "bufconnection.h"

net::Buffer::Buffer(size_t size)
    : _max(size), buf_() {}

void net::Buffer::fill(const void* data, size_t len) {
    const char* ch_data = static_cast<const char*>(data);
    buf_.assign(ch_data, ch_data + std::min(_max, len));
}

void* net::Buffer::data() {
    return buf_.data();
}

const void* net::Buffer::data() const {
    return buf_.data();
}

void net::Buffer::clear() {
    buf_.clear();
}

size_t net::Buffer::size() const {
    return buf_.size();
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
    // write_.data.resize(len);
    // write_.data = (static_cast<const char*>(data));

    connection_.write(data, len);
}
void net::BufferedConnection::read(void* data, size_t len) {
    connection_.read(data, len);
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
    connection_.close();
}

tcp::Descriptor& net::BufferedConnection::fd() {
    return connection_.fd();
}

tcp::Address net::BufferedConnection::adress() const {
    return connection_.address();
}
