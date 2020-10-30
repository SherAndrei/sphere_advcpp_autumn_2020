cmake_minimum_required(VERSION 3.10)
project(LOG)

set(CMAKE_CXX_STANDARD 17)

set(HDR include)
set(LIB lib)
set(TESTS tests)

set(LIB_NAME log)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format -Wpedantic -Wmissing-declarations)

add_library(${LIB_NAME} STATIC ${LIB}/baselogger.cpp ${LIB}/filelogger.cpp ${LIB}/stdoutlogger.cpp ${LIB}/stderrlogger.cpp ${LIB}/level.cpp ${LIB}/logger.cpp ${LIB}/globallogger.cpp)
target_include_directories(${LIB_NAME} PUBLIC include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../${TESTS})

add_executable(test ${TESTS}/test.cpp)
target_link_libraries(test ${LIB_NAME})