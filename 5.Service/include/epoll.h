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
    int mod(tcp::Descripter&& fd);
    int add(tcp::Descripter&& fd);
    int del(tcp::Descripter&& fd);
    void wait(int timeout);
 private:
    std::vector<::epoll_event> event_queue;
    tcp::Descripter epoll_fd_;
};

}  // namespace net

#endif  // NET_EPOLL_H