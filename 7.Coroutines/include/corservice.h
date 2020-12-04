#ifndef HTTP_COR_SERVICE_H
#define HTTP_COR_SERVICE_H
#include "coroutine.h"
#include "corlistener.h"
#include "cormanager.h"
#include "httpservice.h"

namespace http {
namespace cor {

class CoroutineService : protected HttpService {
 public:
    CoroutineService(ICoroutineListener* listener, size_t workersSize);
    virtual ~CoroutineService() = default;

 public:
    void setListener(ICoroutineListener* listener);

 public:
    void run() override;

 private:
    void work(size_t thread_num) override;
    void handle_client(CorConnection* p_client);
    void serve_client(CorConnection* p_client);

 private:
    CorConnection* try_replace_closed_with_new_connection(CorConnection&& cn);
    bool try_read_request(CorConnection* p_client, size_t thread_num);
    bool try_write_responce(CorConnection* p_client);

 private:
    ICoroutineListener* listener_;
    CorManager manager_;
    PtrsToClosedCorConnections closed_;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_SERVICE_H