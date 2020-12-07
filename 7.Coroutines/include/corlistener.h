#ifndef HTTP_COR_LISTENER_H
#define HTTP_COR_LISTENER_H
#include "iListener.h"
#include "message.h"

namespace http {
namespace cor  {

class ICoroutineListener : public net::IListener {
 public:
    virtual Responce OnRequest(const Request&) = 0;
};

}  // namespace cor
}  // namespace http

#endif  // HTTP_COR_LISTENER_H
