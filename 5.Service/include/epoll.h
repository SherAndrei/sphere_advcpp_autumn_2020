#ifndef NET_EPOLL_H
#define NET_EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "option.h"
#include "iConnection.h"
#include "connection_container.h"

namespace net {

class EPoll {
 public:
    EPoll();
    ~EPoll() = default;

    EPoll(const EPoll& other)              = delete;
    EPoll& operator = (const EPoll& other) = delete;

    EPoll(EPoll&& other)              = default;
    EPoll& operator = (EPoll&& other) = default;

 public:
    void add(const tcp::Descriptor& fd, OPTION opt) const;
    void mod(const tcp::Descriptor& fd, OPTION opt) const;

    void add(ConnectionAndData* p_place, OPTION opt) const;
    void mod(ConnectionAndData* p_place, OPTION opt) const;

    void del(const tcp::Descriptor& fd) const;

    std::vector<::epoll_event> wait() const;

    void set_max_events(size_t max_events);

 protected:
    size_t events_queue_size_;
    tcp::Descriptor epoll_fd_;
};

}  // namespace net

#endif  // NET_EPOLL_H
