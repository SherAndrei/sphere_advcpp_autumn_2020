#ifndef NET_EPOLL_H
#define NET_EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "option.h"
#include "descriptor.h"

namespace net {

class EPoll {
 public:
    EPoll();
    ~EPoll() = default;

    EPoll(const EPoll& other)              = delete;
    EPoll& operator = (const EPoll& other) = delete;

    EPoll(EPoll&& other)              = default;
    EPoll& operator = (EPoll&& other) = default;

    void add(const tcp::Descriptor& fd, OPTION opt);
    void mod(const tcp::Descriptor& fd, OPTION opt);
    void del(const tcp::Descriptor& fd);
    std::vector<::epoll_event> wait();

    void set_max_events(size_t max_events);
 private:
    size_t events_queue_size_;
    tcp::Descriptor epoll_fd_;
};

}  // namespace net

#endif  // NET_EPOLL_H
