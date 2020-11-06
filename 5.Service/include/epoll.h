#ifndef NET_EPOLL_H
#define NET_EPOLL_H
#include <sys/epoll.h>
#include <vector>
#include "descripter.h"

namespace net {

class EPoll {
 public:
    static const int MAX_EVENTS = 1000;

    EPoll(/* args */);
    ~EPoll() = default;

    EPoll(const EPoll& other)              = delete;
    EPoll& operator = (const EPoll& other) = delete;

    EPoll(EPoll&& other);
    EPoll& operator = (EPoll&& other);

    // TODO: более сложная логика с передачей итератора?
    void mod(int fd, struct epoll_event* event);
    void add(int fd, struct epoll_event* event);
    void del(int fd, struct epoll_event* event);
    int wait(int timeout);
 private:
    std::vector<::epoll_event> event_queue;
    tcp::Descripter epoll_fd_;
};

}  // namespace net

#endif  // NET_EPOLL_H