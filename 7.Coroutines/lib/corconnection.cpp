#include "corconnection.h"

namespace http {
namespace cor {

CorConnection::CorConnection(tcp::NonBlockConnection&& cn, Routine* p_rout)
    : HttpConnection(std::move(cn))
    , _p_rout(p_rout)
    , is_routine_set(p_rout != nullptr) {}

void CorConnection::set_routine(Routine* p_rout) {
    _p_rout = p_rout;
    is_routine_set = (p_rout != nullptr);
}

Routine* CorConnection::routine() {
    return _p_rout;
}

}  // namespace cor
}  // namespace http
