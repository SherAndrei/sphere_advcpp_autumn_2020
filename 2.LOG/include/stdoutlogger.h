#ifndef LOG_STDOUTLOGGER_H
#define LOG_STDOUTLOGGER_H
#include "baselogger.h"

namespace log {

class StdoutLogger : public BaseLogger {
 public:
    explicit StdoutLogger(LEVEL l = LEVEL::BASE);
    ~StdoutLogger();
};

}  // namespace log

#endif  // LOG_STDOUTLOGGER_H
