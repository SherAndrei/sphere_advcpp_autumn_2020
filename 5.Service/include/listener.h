#ifndef NET_I_SERVICE_LISTENER_H
#define NET_I_SERVICE_LISTENER_H
#include "bufconnection.h"

namespace net {

class IServiceListener {
 public:
    virtual void onNewConnection(const BufferedConnection& cn) = 0;
    virtual void onClose(const BufferedConnection& cn)         = 0;
    virtual void onWriteDone(const BufferedConnection& cn)     = 0;
    virtual void onReadAvailable(const BufferedConnection& cn) = 0;
    virtual void onError(const BufferedConnection& cn)         = 0;
};

}  // namespace net

#endif  // NET_I_SERVICE_LISTENER_H
