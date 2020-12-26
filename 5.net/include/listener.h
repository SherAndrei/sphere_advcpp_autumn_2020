#ifndef NET_I_SERVICE_LISTENER_H
#define NET_I_SERVICE_LISTENER_H
#include "iListener.h"

namespace net {

class BufferedConnection;

class IServiceListener : public IListener {
 public:
    virtual void onNewConnection(BufferedConnection& cn) = 0;
    virtual void onClose(BufferedConnection& cn)         = 0;
    virtual void onWriteDone(BufferedConnection& cn)     = 0;
    virtual void onReadAvailable(BufferedConnection& cn) = 0;
    virtual void onError(BufferedConnection& cn)         = 0;
};

}  // namespace net

#endif  // NET_I_SERVICE_LISTENER_H
