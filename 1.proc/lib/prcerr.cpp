#include "prcerr.h"

namespace prc {

prc::DescriptorError::DescriptorError(const std::string& what, int fd)
    : Error{what}, fd_{fd} {}

int prc::DescriptorError::fd() const {
    return fd_;
}

}  // namespace prc

