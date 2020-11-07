cmake_minimum_required(VERSION 3.10)
project(NET)

set(CMAKE_CXX_STANDARD 17)

set(LIB_NAME net)
set(LIB lib)
set(TESTS tests)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic 
                    -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute
                    -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Wcast-qual -Wno-suggest-attribute=format)

add_library(${LIB_NAME} STATIC ${LIB}/bufconnection.cpp
                               ${LIB}/epoll.cpp
                               ${LIB}/listener.cpp
                               ${LIB}/service.cpp
                               ${LIB}/neterr.cpp)

target_link_directories(${LIB_NAME} PUBLIC ../3.TCP/debug)
target_link_libraries(${LIB_NAME} tcp)
target_include_directories(${LIB_NAME} PUBLIC ../3.TCP/include)
target_include_directories(${LIB_NAME} PUBLIC include)

add_subdirectory(${TESTS})
