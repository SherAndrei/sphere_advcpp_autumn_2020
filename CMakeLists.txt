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

add_subdirectory(1.proc)
add_subdirectory(2.log)
add_subdirectory(3.tcp)
add_subdirectory(4.shmem)
add_subdirectory(5.net)
add_subdirectory(6.http)
add_subdirectory(7.cor)
