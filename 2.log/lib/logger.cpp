#include "logerr.h"
#include "logger.h"

namespace log {

std::unique_ptr<base::BaseLogger> Logger::global_loger_ = nullptr;

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

base::BaseLogger& Logger::get_global_logger() {
    if (global_loger_ == nullptr)
        throw InstanceError("No BaseLogger was initialized");
    return *global_loger_;
}
void Logger::set_global_logger(std::unique_ptr<base::BaseLogger> p_bl) {
    if (!p_bl)
        throw InstanceError("Cannot initialize with nullptr");
    global_loger_ = std::move(p_bl);
}

}  // namespace log
