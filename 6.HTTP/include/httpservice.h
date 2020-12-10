#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <thread>
#include <mutex>
#include "iService.h"
#include "iTimed.h"
#include "httpcontainers.h"
#include "httplistener.h"

namespace http {

class HttpService : public net::IService {
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
    virtual void work(size_t thread_num);
    virtual void activate_workers();

    void subscribe(net::ConnectionPlace* p_place, net::OPTION opt)   const;
    void unsubscribe(net::ConnectionPlace* p_place, net::OPTION opt) const;

    virtual net::ConnectionPlace* emplace_connection(tcp::NonBlockConnection&& cn);
    void close_client(net::ConnectionPlace* p_place);

    void dump_timed_out_connections();
    bool close_if_timed_out(net::ConnectionPlace* p_place);

 private:
    virtual bool try_read_request(net::ConnectionPlace* p_place);
    virtual bool try_write_responce(net::ConnectionPlace* p_place);

 protected:
    virtual net::ConnectionPlace* try_replace_closed_with_new_conn(tcp::NonBlockConnection&& cn);
    bool try_reset_last_activity_time(net::ConnectionPlace* p_place);

 protected:
    PlacesOfClosedConnections closed_;
    TimeOrderedConnections    timeod_;

 protected:
    size_t conn_timeo;
    size_t ka_conn_timeo;
    std::vector<std::thread> workers_;
    std::mutex closing_mutex_;
    std::mutex timeout_mutex_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
