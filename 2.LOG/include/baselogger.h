#ifndef LOG_BASELOGGER_H
#define LOG_BASELOGGER_H
#include <ostream>
#include <string>
#include "level.h"

namespace log {

class BaseLogger {
 public:
    BaseLogger(std::ostream& another, LEVEL l);
    virtual ~BaseLogger() = default;

    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

    void  set_level(LEVEL lev);
    LEVEL level() const;

    void flush();
 protected:
    void log(const std::string& m, LEVEL l);

 protected:
    std::ostream* _out;
    LEVEL _level;
};

}  // namespace log

#endif  // LOG_BASELOGGER_H
