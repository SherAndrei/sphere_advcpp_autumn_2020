#ifndef STDOUTLOGGER_H
#define STDOUTLOGGER_H
#include "baselogger.h"

namespace log
{

class StdoutLogger : public BaseLogger
{
public:
    StdoutLogger(LEVEL l);
    ~StdoutLogger();
};

} // namespace log


#endif // STDOUTLOGGER_H