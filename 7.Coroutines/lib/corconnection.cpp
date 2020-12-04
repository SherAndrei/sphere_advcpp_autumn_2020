#include "corconnection.h"

namespace http {
namespace cor {

CorConnection::CorConnection(tcp::Connection&& cn, routine_t r_id)
    : HttpConnection(std::move(cn)), _r_id(r_id) {}

void CorConnection::set_routine(routine_t id) {
    _r_id = id;
}

routine_t CorConnection::routine_id() const {
    return _r_id;
}

}  // namespace cor
}  // namespace http
