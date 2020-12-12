#ifndef HTTP_COR_WORKER_H
#define HTTP_COR_WORKER_H
#include "httpworker.h"

namespace http {
namespace cor  {

class CoroutineService;

class CoroutineWorker final : protected HttpWorker {
 public:
    using HttpWorker::HttpWorker;

 private:
    friend CoroutineService;
    void set_service_pointer(CoroutineService* p_service);

 private:
    void work() override;
    void serve_client(net::ConnectionAndData* p_place);

    void subscribe(net::ConnectionAndData* p_place, net::OPTION opt)   const override;
    void unsubscribe(net::ConnectionAndData* p_place, net::OPTION opt) const override;

    void close_client(net::ConnectionAndData* p_place)             override;
    void reset_last_activity_time(net::ConnectionAndData* p_place) override;


    bool try_read_request(net::ConnectionAndData* p_place)   override;
    bool try_write_responce(net::ConnectionAndData* p_place) override;

 private:
    CoroutineService* p_service_;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_WORKER_H
