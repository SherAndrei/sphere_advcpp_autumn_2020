#include "stderrlogger.h"
#include <iostream>

using namespace log;

StderrLogger::StderrLogger(LEVEL l)
    : BaseLogger(std::cerr, l) {}