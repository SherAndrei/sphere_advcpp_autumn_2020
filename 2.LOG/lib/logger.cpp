#include "logerr.h"
#include "logger.h"

std::unique_ptr<log::BaseLogger> log::Logger::global_loger_ = nullptr;
std::unique_ptr<log::Logger>     log::Logger::instance_     = nullptr;

log::Logger& log::Logger::get_instance() {
    if (instance_ == nullptr)
        instance_.reset(new Logger);
    return *instance_;
}
log::BaseLogger& log::Logger::get_global_logger() {
    if (global_loger_ == nullptr)
        throw InstanceError("No BaseLogger was initialized");
    return *global_loger_;
}
void log::Logger::set_global_logger(std::unique_ptr<BaseLogger> p_bl) {
    if (global_loger_ != nullptr)
        global_loger_->flush();
    global_loger_ = std::move(p_bl);
}
