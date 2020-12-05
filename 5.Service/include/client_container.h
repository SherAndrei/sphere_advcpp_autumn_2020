#ifndef NET_CONNECTION_MANAGER_H
#define NET_CONNECTION_MANAGER_H
#include <list>
#include <memory>
#include "iConnectable.h"

namespace net {

using ConnectionPtr = std::unique_ptr<tcp::IConnectable>;

struct IClient {
    ConnectionPtr conn{};
    std::list<IClient>::iterator iter{};
};

using ClientContainer = typename std::list<IClient>;

}  // namespace net


#endif  // NET_CONNECTION_MANAGER_H
