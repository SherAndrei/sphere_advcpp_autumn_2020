#include "corconnection.h"

namespace http {
namespace cor {

CorConnection::CorConnection(tcp::NonBlockConnection&& cn, routine_t r_id)
    : HttpConnection(std::move(cn)), _r_id(r_id), is_routine_set(r_id > 0) {}

void CorConnection::set_routine(routine_t id) {
    _r_id = id;
    is_routine_set = (id > 0);
}

routine_t CorConnection::routine_id() const {
    return _r_id;
}

}  // namespace cor
}  // namespace http
