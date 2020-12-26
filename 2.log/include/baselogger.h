#ifndef LOG_BASELOGGER_H
#define LOG_BASELOGGER_H
#include <ostream>
#include <string>
#include <mutex>
#include "level.h"

namespace log {
namespace base {

class BaseLogger {
 protected:
    BaseLogger()                  = delete;
    BaseLogger(const BaseLogger&) = delete;
    BaseLogger& operator=(const BaseLogger&) = delete;

 public:
    virtual ~BaseLogger() = default;

    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

    void  set_level(LEVEL lev);
    LEVEL level() const;

    void flush();

 protected:
    BaseLogger(std::ostream& another, LEVEL l);
    void log(const std::string& m, LEVEL l);

 protected:
    std::mutex mutex_;
    std::ostream* _out;
    LEVEL _level;
};

}  // namespace base
}  // namespace log

#endif  // LOG_BASELOGGER_H
