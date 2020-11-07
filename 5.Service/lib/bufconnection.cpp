#include "bufconnection.h"

net::BufferedConnection::BufferedConnection(tcp::Connection && other)
    : connection_(std::move(other)) {}

tcp::Descriptor& net::BufferedConnection::fd() {
    return connection_.fd();
}
