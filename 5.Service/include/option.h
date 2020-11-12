#ifndef NET_OPTION_H
#define NET_OPTION_H
#include <sys/epoll.h>

namespace net {

enum class OPTION {
    UNKNOW = 0,
    READ   = EPOLLIN,
    WRITE  = EPOLLOUT,
    READ_AND_WRITE = EPOLLIN | EPOLLOUT
};

OPTION operator+(OPTION lhs, OPTION rhs);
OPTION operator-(OPTION lhs, OPTION rhs);

}  // namespace net

#endif  // NET_OPTION_H
