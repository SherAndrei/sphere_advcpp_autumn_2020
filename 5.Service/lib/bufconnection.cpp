#include "bufconnection.h"

net::BufferedConnection::BufferedConnection(tcp::Connection && other)
    : connection_(std::move(other)) {}

void net::BufferedConnection::write_from_buf() {
    connection_.write(read_.data.data(), read_.data.length());
}
void net::BufferedConnection::read_to_buf() {
    read_.data.resize(1024);
    connection_.read(read_.data.data(), read_.data.length());
}

net::Buffer net::BufferedConnection::read_buf() {
    return read_;
}
net::Buffer net::BufferedConnection::write_buf() {
    return write_;
}

tcp::Descriptor& net::BufferedConnection::fd() {
    return connection_.fd();
}
