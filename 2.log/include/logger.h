#ifndef LOG_LOGGER_H
#define LOG_LOGGER_H
#include <memory>
#include "baselogger.h"

namespace log  {

class Logger {
 private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator= (const Logger&) = delete;
 public:
    ~Logger() = default;
    static Logger&     get_instance();
    static base::BaseLogger& get_global_logger();
    static void        set_global_logger(std::unique_ptr<base::BaseLogger>);
 private:
    static std::unique_ptr<base::BaseLogger> global_loger_;
};

}  // namespace log

#endif  // LOG_LOGGER_H
