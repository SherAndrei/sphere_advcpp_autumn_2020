#include <iostream>
#include "stdoutlogger.h"

namespace log {

StdoutLogger::StdoutLogger(LEVEL l)
    : BaseLogger(std::cout, l) {}

StdoutLogger::~StdoutLogger() {
    flush();
}

}  // namespace log
