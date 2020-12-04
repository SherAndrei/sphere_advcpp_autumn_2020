#ifndef HTTP_COR_LISTENER_H
#define HTTP_COR_LISTENER_H
#include "message.h"
#include "httpservice.h"
#include "httpconnection.h"

namespace http {
namespace cor  {

class ICoroutineListener {
 public:
    virtual Responce OnRequest(const Request&) = 0;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_LISTENER_H
