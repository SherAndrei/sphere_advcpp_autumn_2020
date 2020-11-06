#include <fstream>
#include "filelogger.h"

log::FileLogger::FileLogger(const std::string& path, LEVEL l)
    : log::BaseLogger(file, l) {
    file.open(path);
}
