#include <iostream>
#include "stderrlogger.h"

namespace log {

StderrLogger::StderrLogger(LEVEL l)
    : BaseLogger(std::cerr, l) {}

}  // namespace log
