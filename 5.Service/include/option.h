#ifndef NET_OPTION_H
#define NET_OPTION_H
#include <sys/epoll.h>

namespace net {

enum class OPTION : size_t {
    UNKNOWN = 0,
    READ   = EPOLLIN,
    WRITE  = EPOLLOUT,
    CLOSE  = EPOLLRDHUP,
    EXCLUSIVE     = EPOLLEXCLUSIVE,
    ET_ONESHOT = EPOLLET | EPOLLONESHOT
};

OPTION operator+(OPTION lhs, OPTION rhs);
OPTION operator-(OPTION lhs, OPTION rhs);

}  // namespace net

#endif  // NET_OPTION_H
