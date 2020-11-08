#ifndef NET_OPTION_H
#define NET_OPTION_H
#include <sys/epoll.h>

namespace net {

enum class OPTION {
    UNKNOW = -1,
    READ   = EPOLLIN  | EPOLLRDHUP,
    WRITE  = EPOLLOUT | EPOLLRDHUP,
    READ_AND_WRITE = EPOLLIN | EPOLLOUT | EPOLLRDHUP
};

}  // namespace net

#endif  // NET_OPTION_H
