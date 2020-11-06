#include <string>
#include <memory>
#include "globallogger.h"
#include "stdoutlogger.h"
#include "stderrlogger.h"
#include "filelogger.h"
#include "logger.h"
#include "level.h"

void log::init_with_stderr_logger(LEVEL l) {
    Logger::get_instance().set_global_logger(std::make_unique<StderrLogger>(l));
}
void log::init_with_stdout_logger(LEVEL l) {
    Logger::get_instance().set_global_logger(std::make_unique<StdoutLogger>(l));
}
void log::init_with_file_logger(const std::string& path, LEVEL l) {
    Logger::get_instance().set_global_logger(std::make_unique<FileLogger>(path, l));
}
void log::debug(const std::string& message) {
    Logger::get_instance().get_global_logger().debug(message);
}
void log::info(const std::string& message) {
    Logger::get_instance().get_global_logger().info(message);
}
void log::warn(const std::string& message) {
    Logger::get_instance().get_global_logger().warn(message);
}
void log::error(const std::string& message) {
    Logger::get_instance().get_global_logger().error(message);
}
void  log::set_level(LEVEL lev) {
    Logger::get_instance().get_global_logger().set_level(lev);
}
log::LEVEL log::level() {
    return Logger::get_instance().get_global_logger().level();
}
void log::flush() {
    Logger::get_instance().get_global_logger().flush();
}
