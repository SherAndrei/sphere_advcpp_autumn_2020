#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <list>
#include "base_service.h"
#include "timeout.h"
#include "httplistener.h"
#include "httpmanager.h"

namespace http {

class HttpService : public net::BaseService {
 public:
    explicit HttpService(IHttpListener* listener, size_t workersSize);
    virtual ~HttpService() = default;

 public:
    void setListener(IHttpListener* listener);
    size_t connections_size();

 public:
    void open(const tcp::Address& addr) override;
    void run() override;
    void close() override;

 protected:
    virtual void work(size_t thread_num);
    void subscribe(HttpConnection& cn, net::OPTION opt)   const;
    void unsubscribe(HttpConnection& cn, net::OPTION opt) const;

    void close_connection(HttpConnection* cn);
    void close_if_timed_out(HttpConnection* cn);

 private:
    HttpConnection* try_replace_closed_with_new_connection(HttpConnection&& cn);
    bool try_read_request(HttpConnection* p_client, size_t thread_num);
    bool try_write_responce(HttpConnection* p_client);
    bool try_reset_last_activity_time(HttpConnection* p_client);

 private:
    IHttpListener* listener_{nullptr};
    HttpManager manager_;
    PtrsToClosedHttpConnections closed_;

 protected:
    std::vector<std::thread> workers_;
    std::mutex mutex_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
