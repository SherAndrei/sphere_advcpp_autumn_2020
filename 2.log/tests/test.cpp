#include "stderrlogger.h"
#include "stdoutlogger.h"
#include "filelogger.h"
#include "globallogger.h"

int main() {
    {
        log::StdoutLogger lg(log::LEVEL::WARN);
        lg.debug("Hello world!");
        lg.warn("Hello world!");
        lg.flush();
        lg.info("Hello world!");
        lg.error("Hello world!");
    }

    {
        log::FileLogger lg("log.txt", log::LEVEL::DEBUG);
        lg.debug("Hello world!");
        lg.warn("Hello world!");
        lg.flush();
        lg.info("Hello world!");
        lg.error("Hello world!");
    }
    {
        log::StderrLogger lg(log::LEVEL::WARN);
        lg.debug("Hello world!");
        lg.warn("Hello world!");
        lg.flush();
        lg.info("Hello world!");
        lg.error("Hello world!");
    }
    {
        log::init_with_file_logger("globallog.txt", log::LEVEL::INFO);
        log::error("Feeling good");
    }
}
