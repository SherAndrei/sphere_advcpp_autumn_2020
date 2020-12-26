#include "logerr.h"

namespace log {

FileLoggerError::FileLoggerError(const std::string& msg, const std::string& filename)
    : Error{msg}, _filename{filename} {}

std::string FileLoggerError::filename() const {
    return _filename;
}

}  // namespace log
