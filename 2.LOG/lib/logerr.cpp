#include "logerr.h"

log::InstanceError::InstanceError(const std::string& msg)
    : Error{msg} {}

log::FileLoggerError::FileLoggerError(const std::string& msg, const std::string& filename)
    : Error{msg}, _filename{filename} {}

std::string log::FileLoggerError::filename() const {
    return _filename;
}
