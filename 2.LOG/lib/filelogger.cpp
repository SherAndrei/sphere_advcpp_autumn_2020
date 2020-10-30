#include "filelogger.h"
#include <fstream>

using namespace log;

FileLogger::FileLogger(const std::string& path, LEVEL l)
    : BaseLogger(file, l)
{ file.open(path); }