#include "error.h"
#include "logger.h"

using namespace log;

std::unique_ptr<BaseLogger> Logger::global_loger_ = nullptr;
std::unique_ptr<Logger>     Logger::instance_     = nullptr;

Logger&     Logger::get_instance() {
    if(instance_ == nullptr) 
        instance_.reset(new Logger);
    return *instance_; 
}
BaseLogger& Logger::get_global_logger() {
    if(global_loger_ == nullptr)
        throw InstanceError("No BaseLogger was initialized");
    return *global_loger_;
}
void Logger::set_global_logger(std::unique_ptr<BaseLogger> p_bl) {
    if(global_loger_ != nullptr)
        global_loger_->flush();
    global_loger_ = std::move(p_bl);
}
