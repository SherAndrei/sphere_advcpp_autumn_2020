#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include "address.h"
#include "worker.h"
#include "service.h"
#include "epoll.h"
#include "httplistener.h"
#include "httpconnection.h"

namespace http {

class HttpService : public net::Service {
 public:
    explicit HttpService(IHttpListener* listener);
    void setListener(IHttpListener* listener);
    void setWorkersSize(size_t size);
    void open(const tcp::Address& addr) override;
    void run()  override;
 private:
    std::vector<Worker> workers_;
    IHttpListener* listener_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
