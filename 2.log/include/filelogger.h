#ifndef LOG_FILELOGGER_H
#define LOG_FILELOGGER_H
#include <string>
#include <fstream>
#include "baselogger.h"

namespace log {

class FileLogger : public base::BaseLogger {
 public:
    explicit FileLogger(const std::string& path, LEVEL l = LEVEL::DEBUG);
    ~FileLogger() = default;
 private:
    std::ofstream file;
};

}  // namespace log

#endif  // LOG_FILELOGGER_H
