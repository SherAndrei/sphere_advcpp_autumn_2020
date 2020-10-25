#include <string>
#include <memory>
#include "globallogger.h"
#include "stdoutlogger.h"
#include "stderrlogger.h"
#include "filelogger.h"
#include "logger.h"
#include "level.h"

using namespace log;
void log::init_with_stderr_logger(LEVEL l)
{
    Logger& lg = Logger::get_instance();
    std::unique_ptr<BaseLogger> pbl(new StderrLogger(l));
    lg.set_global_logger(std::move(pbl));
}
void log::init_with_stdout_logger(LEVEL l)
{
    Logger& lg = Logger::get_instance();
    std::unique_ptr<BaseLogger> pbl(new StdoutLogger(l));
    lg.set_global_logger(std::move(pbl));
}
void log::init_with_file_logger(const std::string& path, LEVEL l)
{
    Logger& lg = Logger::get_instance();
    std::unique_ptr<BaseLogger> pbl(new FileLogger(path, l));
    lg.set_global_logger(std::move(pbl));
}
void log::debug(const std::string& message)
{
    Logger& lg = Logger::get_instance();
    lg.get_global_logger().debug(message);
}
void log::info (const std::string& message)
{
    Logger& lg = Logger::get_instance();
    lg.get_global_logger().info(message);
}
void log::warn (const std::string& message)
{
    Logger& lg = Logger::get_instance();
    lg.get_global_logger().warn(message);
}
void log::error(const std::string& message)
{
    Logger& lg = Logger::get_instance();
    lg.get_global_logger().error(message);
}

void  log::set_level(LEVEL lev)
{
    Logger& lg = Logger::get_instance();
    lg.get_global_logger().set_level(lev);
}
LEVEL log::level()
{
    Logger& lg = Logger::get_instance();
    return lg.get_global_logger().level();
}
void log::flush()
{
    Logger& lg = Logger::get_instance();
    lg.get_global_logger().flush();
}
