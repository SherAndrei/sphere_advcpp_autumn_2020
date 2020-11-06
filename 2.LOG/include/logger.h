#ifndef LOG_LOGGER_H
#define LOG_LOGGER_H
#include <memory>
#include "baselogger.h"

namespace log  {
// singleton
class Logger {
 private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator= (const Logger&) = delete;
 public:
    ~Logger() = default;
    static Logger&     get_instance();
    static BaseLogger& get_global_logger();
    static void        set_global_logger(std::unique_ptr<BaseLogger>);
 private:
    static std::unique_ptr<BaseLogger> global_loger_;
    static std::unique_ptr<Logger>     instance_;
};

}  // namespace log

#endif  // LOG_LOGGER_H
