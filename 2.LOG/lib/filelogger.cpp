#include <fstream>
#include "logerr.h"
#include "filelogger.h"

namespace log {

FileLogger::FileLogger(const std::string& path, LEVEL l)
    : BaseLogger(file, l), file(path) {
    if (!file)
        throw FileLoggerError("Cannot open file", path);
}

}  // namespace log
