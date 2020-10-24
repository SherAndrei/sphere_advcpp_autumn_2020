#ifndef STDERRLOGGER_H
#define STDERRLOGGER_H
#include "baselogger.h"

namespace log
{

class StderrLogger : public BaseLogger
{
public:
    StderrLogger(LEVEL l);
};

} // namespace log


#endif // STDERRLOGGER_H