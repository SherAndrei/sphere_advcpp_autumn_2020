#include "corconnection.h"

namespace http {
namespace cor {

CorConnection::CorConnection(tcp::Connection&& cn, routine_t r_id)
    : HttpConnection(std::move(cn)), _r_id(r_id) {}

routine_t CorConnection::routine_id() const {
    return _r_id;
}

}  // namespace cor
}  // namespace http
