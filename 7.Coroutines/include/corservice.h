#ifndef HTTP_COR_SERVICE_H
#define HTTP_COR_SERVICE_H
#include <vector>
#include "coroutine.h"
#include "corlistener.h"
#include "corworker.h"
#include "httpservice.h"

namespace http {
namespace cor {

class CoroutineService final : protected HttpService {
 public:
    CoroutineService(const tcp::Address& addr, ICoroutineListener* listener, size_t workersSize,
                     size_t connection_timeout_sec = CONNECTION_TIMEOUT,
                     size_t keep_alive_timeout_sec = KEEP_ALIVE_CONNECTION_TIMEOUT);

 private:
    friend class CoroutineWorker;
    void activate_workers() override;

 public:
    void run() override;

 public:
    ICoroutineListener* getListener();
    void setListener(ICoroutineListener* listener);

 private:
    net::ConnectionAndData* emplace_connection(tcp::NonBlockConnection&& cn) override;
    net::ConnectionAndData* try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn) override;

    bool close_if_timed_out(net::ConnectionAndData* p_place) override;

 private:
    std::vector<CoroutineWorker> workers_;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_SERVICE_H
