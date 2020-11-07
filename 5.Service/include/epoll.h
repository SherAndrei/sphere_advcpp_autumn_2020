#ifndef NET_EPOLL_H
#define NET_EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "descriptor.h"

namespace net {

class EPoll {
 public:
    EPoll(/* args */);
    ~EPoll() = default;

    EPoll(const EPoll& other)              = delete;
    EPoll& operator = (const EPoll& other) = delete;

    EPoll(EPoll&& other);
    EPoll& operator = (EPoll&& other);

    void mod(const tcp::Descriptor& fd);
    void add(const tcp::Descriptor& fd);
    void del(const tcp::Descriptor& fd);
    std::vector<::epoll_event> wait();

    void set_max_events(size_t max_events);
 private:
    std::vector<::epoll_event> event_queue;
    tcp::Descriptor epoll_fd_;
};

}  // namespace net

#endif  // NET_EPOLL_H
