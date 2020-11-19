#include <string>
#include <memory>
#include "globallogger.h"
#include "stdoutlogger.h"
#include "stderrlogger.h"
#include "filelogger.h"
#include "logger.h"
#include "level.h"

namespace log {

void init_with_stderr_logger(LEVEL l) {
    Logger::get_instance().set_global_logger(std::make_unique<StderrLogger>(l));
}
void init_with_stdout_logger(LEVEL l) {
    Logger::get_instance().set_global_logger(std::make_unique<StdoutLogger>(l));
}
void init_with_file_logger(const std::string& path, LEVEL l) {
    Logger::get_instance().set_global_logger(std::make_unique<FileLogger>(path, l));
}
void debug(const std::string& message) {
    Logger::get_instance().get_global_logger().debug(message);
}
void info(const std::string& message) {
    Logger::get_instance().get_global_logger().info(message);
}
void warn(const std::string& message) {
    Logger::get_instance().get_global_logger().warn(message);
}
void error(const std::string& message) {
    Logger::get_instance().get_global_logger().error(message);
}
void  set_level(LEVEL lev) {
    Logger::get_instance().get_global_logger().set_level(lev);
}
LEVEL level() {
    return Logger::get_instance().get_global_logger().level();
}
void flush() {
    Logger::get_instance().get_global_logger().flush();
}

}  // namespace log
