#ifndef FILELOGGER_H
#define FILELOGGER_H
#include "baselogger.h"
#include <fstream>

namespace log
{
    
class FileLogger : public BaseLogger
{
public:
    FileLogger(const std::string& path, LEVEL l);
    ~FileLogger() override;

private:
    std::ofstream file;
};

} // namespace log
#endif // FILELOGGER_H