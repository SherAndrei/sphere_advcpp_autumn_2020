#include <fstream>
#include "logerr.h"
#include "filelogger.h"

log::FileLogger::FileLogger(const std::string& path, LEVEL l)
    : log::BaseLogger(file, l), file(path) {
    if (!file)
        throw FileLoggerError("Cannot open file", path);
}
