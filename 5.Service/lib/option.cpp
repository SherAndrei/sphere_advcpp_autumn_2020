#include "option.h"

namespace net {

OPTION operator+(OPTION lhs, OPTION rhs) {
    unsigned i_lhs = static_cast<unsigned>(lhs);
    unsigned i_rhs = static_cast<unsigned>(rhs);
    return static_cast<OPTION>(i_lhs | i_rhs);
}

OPTION operator-(OPTION lhs, OPTION rhs) {
    unsigned i_lhs = static_cast<unsigned>(lhs);
    unsigned i_rhs = static_cast<unsigned>(rhs);
    return static_cast<OPTION>(i_lhs & ~i_rhs);
}

}  // namespace net
