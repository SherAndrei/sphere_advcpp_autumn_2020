#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H
#include <vector>
#include <thread>
#include <mutex>
#include "iService.h"
#include "httpcontainers.h"
#include "timeout.h"
#include "httplistener.h"

namespace http {

class HttpService : public net::IService {
 public:
    explicit HttpService(const tcp::Address& addr, IHttpListener* listener, size_t workersSize,
                         size_t connection_timeout_sec = CONNECTION_TIMEOUT,
                         size_t keep_alive_timeout_sec = KEEP_ALIVE_CONNECTION_TIMEOUT);
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
    void subscribe(net::IClient* cn, net::OPTION opt)   const;
    void unsubscribe(net::IClient* cn, net::OPTION opt) const;

    net::IClient* add_new_connection(tcp::NonBlockConnection&& cn);
    void close_client(net::IClient* p_client);

    void dump_timed_out_connections();
    bool close_if_timed_out(net::IClient* p_client);

 private:
    virtual bool try_read_request(net::IClient* p_client, size_t thread_num);
    virtual bool try_write_responce(net::IClient* p_client);

 protected:
    bool try_reset_last_activity_time(net::IClient* p_client);

 private:
    IHttpListener* listener_{nullptr};

 protected:
    PtrsToClosedClients closed_;
    size_t conn_timeo;
    size_t ka_conn_timeo;
    std::vector<std::thread> workers_;
    std::mutex closing_mutex_;
    std::mutex timeout_mutex_;
};

}  // namespace http

#endif  // HTTP_SERVICE_H
