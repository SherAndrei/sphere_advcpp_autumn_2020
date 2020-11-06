#include <iostream>
#include "stdoutlogger.h"

log::StdoutLogger::StdoutLogger(LEVEL l)
    : BaseLogger(std::cout, l) {}

log::StdoutLogger::~StdoutLogger() {
    flush();
}
