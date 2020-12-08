#ifndef NET_CONNECTION_MANAGER_H
#define NET_CONNECTION_MANAGER_H
#include <list>
#include <memory>
#include "iConnectable.h"

namespace net {

using ConnectionUPtr = std::unique_ptr<tcp::IConnectable>;

struct IClient {
    ConnectionUPtr conn{};
    std::list<IClient>::iterator iter{};
};

struct IClientPlace {
    IClient* p_client;
    std::list<IClientPlace>::iterator iter{};
};

using ClientContainer = typename std::list<IClient>;
using ClientPlaces    = typename std::list<IClientPlace>;

}  // namespace net


#endif  // NET_CONNECTION_MANAGER_H
