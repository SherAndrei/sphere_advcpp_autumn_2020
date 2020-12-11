#ifndef HTTP_WORKER_H
#define HTTP_WORKER_H
#include <thread>
#include <string>
#include "option.h"
#include "connection_container.h"

namespace http {

class HttpService;

class HttpWorker {
 public:
    HttpWorker()                          = default;
    HttpWorker(HttpWorker&&)              = default;
    HttpWorker& operator = (HttpWorker&&) = default;
    virtual ~HttpWorker()                 = default;

 public:
    virtual void work();
    void join();

    std::string info() const;

 protected:
    friend class HttpService;
    void set_thread(std::thread&& thread);
    void set_thread_num(size_t thread_num);
    void set_service_pointer(HttpService* p_service);

 protected:
    virtual void subscribe(net::ConnectionAndData* p_place, net::OPTION opt)   const;
    virtual void unsubscribe(net::ConnectionAndData* p_place, net::OPTION opt) const;

 private:
    virtual bool try_read_request(net::ConnectionAndData* p_place);
    virtual bool try_write_responce(net::ConnectionAndData* p_place);

    void reset_last_activity_time(net::ConnectionAndData* p_place);
    void close_client(net::ConnectionAndData* p_place);


 protected:
    HttpService* p_service_{nullptr};
    size_t thread_num_{0ul};
    std::thread thread_{};
};

}  // namespace http


#endif  // HTTP_WORKER_H
