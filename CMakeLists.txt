cmake_minimum_required(VERSION 3.10)
project(TechnoSphere)

set(CMAKE_CXX_STANDARD 17)

add_compile_options( -W  -Wall      -Werror         -pedantic
                     -Wextra        -Wfloat-equal   -pedantic-errors
                     -Wcast-align   -Wpointer-arith -Wformat-security
                     -Wformat       -Wwrite-strings -Wcast-align
                     -Wno-long-long -Wunused        -Wcast-qual
                     -Wmissing-declarations
                     -fstack-protector-all)

include_directories(utils)

add_subdirectory(1.Process)
add_subdirectory(2.LOG)
add_subdirectory(3.TCP)
add_subdirectory(4.SharedMap)
add_subdirectory(5.Service)
add_subdirectory(6.HTTP)
add_subdirectory(7.Coroutines)
