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

void net::EPoll::mod(int fd, struct epoll_event* event) {
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_MOD,
                           fd, event));
}
void net::EPoll::add(int fd, struct epoll_event* event) {
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_ADD,
                           fd, event));
}
void net::EPoll::del(int fd, struct epoll_event* event) {
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_DEL,
                 fd, event));
}

int net::EPoll::wait(int timeout) {
    int result;
    handle_error(result = ::epoll_wait(epoll_fd_.fd(),
                 event_queue.data(), event_queue.size(), timeout));
    return result;
}
