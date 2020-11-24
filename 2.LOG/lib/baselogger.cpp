#include <memory>
#include <ostream>
#include "level.h"
#include "baselogger.h"

namespace log {
namespace base {

BaseLogger::BaseLogger(std::ostream& another, LEVEL l)
    : _out(&another), _level(l) {}

void  BaseLogger::debug(const std::string& message) {
    log("[DEBUG]: " + message, LEVEL::DEBUG);
}
void  BaseLogger::info(const std::string& message) {
    log("[INFO]: "  + message, LEVEL::INFO);
}
void  BaseLogger::warn(const std::string& message) {
    log("[WARNING]: " + message, LEVEL::WARN);
}
void  BaseLogger::error(const std::string& message) {
    log("[ERROR]: " + message, LEVEL::ERROR);
}
void  BaseLogger::set_level(LEVEL lev) {
    _level = lev;
}
LEVEL BaseLogger::level() const {
    return _level;
}
void BaseLogger::flush() {
    _out->flush();
}
void BaseLogger::log(const std::string& m, LEVEL l) {
    if (l >= _level)
        (*_out) << m << std::endl;
}

}  // namespace base
}  // namespace log
