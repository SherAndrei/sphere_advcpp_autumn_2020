add_library(log STATIC baselogger.cpp
                       filelogger.cpp
                       stdoutlogger.cpp
                       stderrlogger.cpp
                       globallogger.cpp
                       logger.cpp
                       logerr.cpp)

target_include_directories(log PUBLIC ${PROJECT_SOURCE_DIR}/include)
