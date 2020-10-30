#include "stdoutlogger.h"
#include <iostream>

using namespace log;

StdoutLogger::StdoutLogger(LEVEL l)
    : BaseLogger(std::cout, l)
{}

StdoutLogger::~StdoutLogger() { flush(); }