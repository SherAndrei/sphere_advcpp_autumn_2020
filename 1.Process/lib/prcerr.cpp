#include "prcerr.h"

prc::CreationError::CreationError(const std::string& what)
    : Error{what} {}

prc::DescriptorError::DescriptorError(const std::string& what, int fd)
    : Error{what}, fd_{fd} {}

int prc::DescriptorError::fd() const {
    return fd_;
}
