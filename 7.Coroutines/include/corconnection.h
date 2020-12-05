#ifndef HTTP_COR_CONNECTION_H
#define HTTP_COR_CONNECTION_H
#include <string>
#include "coroutine.h"
#include "httpconnection.h"

namespace http {
namespace cor {

class CorConnection : public HttpConnection {
 public:
    CorConnection(tcp::NonBlockConnection&& cn, routine_t r_id);

 public:
    routine_t routine_id() const;

 private:
    friend class CoroutineService;
    using HttpConnection::HttpConnection;

    void set_routine(routine_t id);

 private:
    routine_t _r_id;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_CONNECTION_H
