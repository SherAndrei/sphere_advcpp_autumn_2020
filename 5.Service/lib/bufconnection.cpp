#include "bufconnection.h"

net::BufferedConnection::BufferedConnection(tcp::Connection && other, EPoll* p_epoll)
    : connection_(std::move(other)), p_epoll_(p_epoll) {}

void net::BufferedConnection::write(const void* data, size_t len) {
    // write_.data.resize(len);
    // write_.data = (static_cast<const char*>(data));

    connection_.write(data, len);
}
void net::BufferedConnection::read(void* data, size_t len) {
    // read_.data.resize(len);
    // read_.data = (static_cast<char*>(data));
    connection_.read(data, len);
}

void net::BufferedConnection::subscribe(OPTION opt) {
    OPTION t_opt = p_epoll_->option();
    unsigned t_optint = static_cast<unsigned>(t_opt);
    unsigned optint   = static_cast<unsigned>(opt);
    t_opt = static_cast<OPTION>(t_optint | optint);
    p_epoll_->set_option(t_opt);
    p_epoll_->add(connection_.fd());
}
void net::BufferedConnection::unsubscribe(OPTION opt) {
    OPTION t_opt = p_epoll_->option();
    unsigned t_optint = static_cast<unsigned>(t_opt);
    unsigned optint   = static_cast<unsigned>(opt);
    t_opt = static_cast<OPTION>(t_optint & ~optint);
    p_epoll_->set_option(t_opt);
    p_epoll_->mod(connection_.fd());
}

net::Buffer net::BufferedConnection::read_buf() {
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
