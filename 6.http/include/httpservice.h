#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <mutex>
#include "BaseService.h"
#include "httpworker.h"
#include "httpcontainers.h"
#include "httplistener.h"

namespace http {

class HttpService : public net::BaseService {
 public:
    HttpService(const tcp::Address& addr, IHttpListener* listener, size_t workersSize,
                size_t connection_timeout_sec = CONNECTION_TIMEOUT,
                size_t keep_alive_timeout_sec = KEEP_ALIVE_CONNECTION_TIMEOUT);

    virtual ~HttpService() = default;

 public:
    void setListener(IHttpListener* listener);
    IHttpListener* getListener();
    size_t connections_size();

 public:
    void open(const tcp::Address& addr) override;
    void run() override;
    void close() override;

 protected:
    virtual void activate_workers();

    virtual net::ConnectionAndData* emplace_connection(tcp::NonBlockConnection&& cn);
    void dump_timed_out_connections();
    virtual bool close_if_timed_out(net::ConnectionAndData* p_place);

 protected:
    virtual net::ConnectionAndData* try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn);

 protected:
    friend class HttpWorker;
    PlacesOfClosedConnections closed_;
    TimeOrderedConnections    timeod_;

 protected:
    size_t conn_timeo;
    size_t ka_conn_timeo;
    std::mutex closing_mutex_;
    std::mutex timeout_mutex_;

 private:
    std::vector<HttpWorker> workers_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
