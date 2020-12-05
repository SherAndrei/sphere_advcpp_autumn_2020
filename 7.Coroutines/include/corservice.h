#ifndef HTTP_COR_SERVICE_H
#define HTTP_COR_SERVICE_H
#include "client_container.h"
#include "coroutine.h"
#include "corlistener.h"
#include "httpservice.h"

namespace http {
namespace cor {

class CoroutineService : public HttpService {
 public:
    CoroutineService(ICoroutineListener* listener, size_t workersSize);
    virtual ~CoroutineService() = default;

 public:
    void setListener(ICoroutineListener* listener);

 public:
    void run() override;

 private:
    void work(size_t thread_num) override;
    void serve_client(net::IClient* p_client);

 private:
    bool try_read_request(net::IClient* p_client, size_t thread_num) override;
    bool try_write_responce(net::IClient* p_client) override;

 private:
    ICoroutineListener* listener_;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_SERVICE_H
