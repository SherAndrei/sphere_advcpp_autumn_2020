#include <iostream>
#include "stderrlogger.h"

log::StderrLogger::StderrLogger(LEVEL l)
    : BaseLogger(std::cerr, l) {}
