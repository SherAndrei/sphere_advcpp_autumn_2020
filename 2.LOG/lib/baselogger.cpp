#include <memory>
#include <ostream>
#include "level.h"
#include "baselogger.h"

log::BaseLogger::BaseLogger(std::ostream& another, LEVEL l)
    : _out(&another), _level(l) {}

void  log::BaseLogger::debug(const std::string& message) {
    log("[DEBUG]: " + message, LEVEL::DEBUG);
}
void  log::BaseLogger::info(const std::string& message) {
    log("[INFO]: "  + message, LEVEL::INFO);
}
void  log::BaseLogger::warn(const std::string& message) {
    log("[WARNING]: " + message, LEVEL::WARN);
}
void  log::BaseLogger::error(const std::string& message) {
    log("[ERROR]: " + message, LEVEL::ERROR);
}
void  log::BaseLogger::set_level(LEVEL lev) {
    _level = lev;
}
log::LEVEL log::BaseLogger::level() const {
    return _level;
}
void log::BaseLogger::flush() {
    _out->flush();
}
void log::BaseLogger::log(const std::string& m, LEVEL l) {
    if (l >= _level)
        (*_out) << m << std::endl;
}
