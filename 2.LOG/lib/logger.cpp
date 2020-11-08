#include "logerr.h"
#include "logger.h"

std::unique_ptr<log::BaseLogger> log::Logger::global_loger_ = nullptr;

log::Logger& log::Logger::get_instance() {
    static Logger instance;
    return instance;
}
log::BaseLogger& log::Logger::get_global_logger() {
    if (global_loger_ == nullptr)
        throw InstanceError("No BaseLogger was initialized");
    return *global_loger_;
}
void log::Logger::set_global_logger(std::unique_ptr<BaseLogger> p_bl) {
    if (!p_bl)
        throw InstanceError("Cannot initialize with nullptr");
    if (global_loger_ != nullptr)
        global_loger_->flush();
    global_loger_ = std::move(p_bl);
}
