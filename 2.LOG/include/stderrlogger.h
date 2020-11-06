#ifndef LOG_STDERRLOGGER_H
#define LOG_STDERRLOGGER_H
#include "baselogger.h"

namespace log {

class StderrLogger : public BaseLogger {
 public:
    explicit StderrLogger(LEVEL l);
    ~StderrLogger() = default;
};

}  // namespace log

#endif  // LOG_STDERRLOGGER_H
