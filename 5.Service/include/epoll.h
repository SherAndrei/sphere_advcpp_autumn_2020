#ifndef NET_EPOLL_H
#define NET_EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "descriptor.h"

namespace net {

enum class OPTION {
    UNKNOW = -1,
    READ   = EPOLLIN  | EPOLLRDHUP,
    WRITE  = EPOLLOUT | EPOLLRDHUP,
    READ_AND_WRITE = EPOLLIN | EPOLLOUT | EPOLLRDHUP
};

class EPoll {
 public:
    EPoll();
    ~EPoll() = default;

    EPoll(const EPoll& other)              = delete;
    EPoll& operator = (const EPoll& other) = delete;

    EPoll(EPoll&& other) = default;
    EPoll& operator = (EPoll&& other) = default;

    void mod(const tcp::Descriptor& fd);
    void add(const tcp::Descriptor& fd);
    void del(const tcp::Descriptor& fd);
    std::vector<::epoll_event> wait();

    void set_max_events(size_t max_events);
    void set_option(OPTION other);
 private:
    OPTION _opt = OPTION::READ;
    std::vector<::epoll_event> event_queue;
    tcp::Descriptor epoll_fd_;
};

}  // namespace net

#endif  // NET_EPOLL_H
