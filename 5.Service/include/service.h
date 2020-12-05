#ifndef NET_SERVICE_H
#define NET_SERVICE_H
#include "iService.h"
#include "listener.h"

namespace net {

class Service : public IService {
 public:
    Service(IServiceListener* listener, const tcp::Address& addr);
    void setListener(IServiceListener* listener);

 public:
    void open(const tcp::Address& addr) override;
    void run() override;
    void close() override;

 private:
    IServiceListener* listener_{nullptr};
};

}  // namespace net

#endif  // NET_SERVICE_H
