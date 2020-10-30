#ifndef BASELOGGER_H
#define BASELOGGER_H
#include <ostream>
#include <memory>
#include "level.h"

namespace log
{

class BaseLogger
{
public:
    BaseLogger(std::ostream& another, LEVEL l);
    virtual ~BaseLogger() = default;

    void debug(const std::string& message);
    void info (const std::string& message);
    void warn (const std::string& message);
    void error(const std::string& message);

    void  set_level(LEVEL lev);
    LEVEL level() const;

    void flush();
protected:
    void log(const std::string& m, LEVEL l);
protected:
    // хотим чтобы поток жил пока жив baselogger
    std::shared_ptr<std::ostream> _out;
    LEVEL _level;
};
	
} // namespace log

#endif // BASELOGGER_H