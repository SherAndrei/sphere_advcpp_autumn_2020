#ifndef LOGGER_H
#define LOGGER_H

namespace log 
{
// singleton
class Logger
{
    Logger(const Logger&) = delete;
    Logger& operator= (const Logger) = delete;  
public:
};

}


#endif // LOGGER_H 