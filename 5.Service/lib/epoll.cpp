#include <cstring>
#include "epoll.h"
#include "neterr.h"

static void handle_error(int err) {
    if (err < 0) {
        throw net::EPollError(std::strerror(errno));
    }
}
static const int MAX_EVENTS = 1000;

net::EPoll::EPoll() : event_queue(MAX_EVENTS), epoll_fd_(::epoll_create(1)) {
    handle_error(epoll_fd_.fd());
}

void net::EPoll::set_option(OPTION other) {
    _opt = other;
}

void net::EPoll::set_max_events(size_t max_events) {
    event_queue.resize(max_events);
}

void net::EPoll::mod(const tcp::Descriptor& d) {
    ::epoll_event event{};
    event.events  = static_cast<uint32_t>(_opt);
    event.data.fd = d.fd();
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_MOD,
                           d.fd(), &event));
}
void net::EPoll::add(const tcp::Descriptor& d) {
    ::epoll_event event{};
    event.events  = static_cast<uint32_t>(_opt);
    event.data.fd = d.fd();
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_ADD,
                           d.fd(), &event));
}
void net::EPoll::del(const tcp::Descriptor& d) {
    ::epoll_event event{};
    event.events  = static_cast<uint32_t>(_opt);
    event.data.fd = d.fd();
    handle_error(epoll_ctl(epoll_fd_.fd(), EPOLL_CTL_DEL,
                 d.fd(), &event));
}

std::vector<::epoll_event> net::EPoll::wait() {
    int events_count;
    handle_error(events_count = ::epoll_wait(epoll_fd_.fd(),
                 event_queue.data(), event_queue.size(), -1));
    return { event_queue.begin(), event_queue.begin() + events_count };
}
