#ifndef LOG_FILELOGGER_H
#define LOG_FILELOGGER_H
#include <string>
#include <fstream>
#include "baselogger.h"

namespace log {

class FileLogger : public BaseLogger {
 public:
    FileLogger(const std::string& path, LEVEL l);
    ~FileLogger() = default;
 private:
    std::ofstream file;
};

}  // namespace log

#endif  // LOG_FILELOGGER_H
