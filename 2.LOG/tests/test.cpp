#include "stderrlogger.h"
#include "stdoutlogger.h"
#include "filelogger.h"

int main()
{
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
}