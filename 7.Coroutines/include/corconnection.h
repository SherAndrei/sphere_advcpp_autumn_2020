#ifndef HTTP_COR_CONNECTION_H
#define HTTP_COR_CONNECTION_H
#include <string>
#include "coroutine.h"
#include "httpconnection.h"

namespace http {
namespace cor {

struct Routine;

class CorConnection final : public HttpConnection {
 public:
    CorConnection(tcp::NonBlockConnection&& cn, Routine* p_rout);

 public:
    Routine* routine();

 private:
    friend class CoroutineService;
    friend class CoroutineWorker;
    void set_routine(Routine* p_rout);

 private:
    Routine* _p_rout = nullptr;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_CONNECTION_H
