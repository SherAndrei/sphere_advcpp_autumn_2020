#ifndef NET_SERVICE_H
#define NET_SERVICE_H
#include "address.h"
#include "listener.h"
#include "base_service.h"
#include "connection_manager.h"

namespace net {

class Service : public BaseService {
 public:
    explicit Service(IServiceListener* listener);
    void setListener(IServiceListener* listener);
 
 public:
    void open(const tcp::Address& addr) override;
    void run() override;
    void close() override;

 private:
    IServiceListener* listener_{nullptr};
    ConnectionManager manager_;
};

}  // namespace net

#endif  // NET_SERVICE_H
