#include <cstring>
#include "epoll.h"
#include "neterr.h"

static void handle_error(int err) {
    if (err < 0) {
        throw net::EPollError(std::strerror(errno));
    }
}

net::EPoll::EPoll() : event_queue(MAX_EVENTS), epoll_fd_(::epoll_create(1)) {
    handle_error(epoll_fd_.fd());
}

int net::EPoll::mod(tcp::Descripter&& fd) {
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_MOD,
                           fd.fd(), NULL /*FIXME*/));
}
int net::EPoll::add(tcp::Descripter&& fd) {
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_ADD,
                           fd.fd(), NULL /*FIXME*/));
}
int net::EPoll::del(tcp::Descripter&& fd) {
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_DEL,
                 fd.fd(), NULL /*FIXME*/));
}

void net::EPoll::wait(int timeout) {
    ::epoll_wait(epoll_fd_.fd(),
                 event_queue.data(), event_queue.size(), timeout);
}

