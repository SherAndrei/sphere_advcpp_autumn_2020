cmake_minimum_required(VERSION 3.10)
project(TCP)

set(CMAKE_CXX_STANDARD 17)

set(HDR hdr)
set(SRC src)
set(TESTDIR tests)

include_directories( ${HDR} )
FILE(GLOB SRCS ${SRC}/connection.cpp ${SRC}/descripter.cpp ${SRC}/error.cpp ${SRC}/server.cpp)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format-Werror -Wpedantic)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../${TESTDIR})

foreach(test testServer testClient)
    add_executable(${test} ${TESTDIR}/${test}.cpp ${SRCS})
endforeach(test)