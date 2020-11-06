#ifndef LOG_GLOBALLOG_H
#define LOG_GLOBALLOG_H
#include <string>
#include "level.h"

namespace log {
void init_with_stderr_logger(LEVEL l);
void init_with_stdout_logger(LEVEL l);
void init_with_file_logger(const std::string& path, LEVEL l);

void debug(const std::string& message);
void info(const std::string& message);
void warn(const std::string& message);
void error(const std::string& message);

void  set_level(LEVEL lev);
LEVEL level();

void flush();
}  // namespace log

#endif  // LOG_GLOBALLOG_H
