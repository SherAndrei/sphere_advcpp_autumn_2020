cmake_minimum_required(VERSION 3.10)
project(TechnoSphere)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format -Wpedantic -Wmissing-declarations)

include_directories(utils)

add_subdirectory(1.Process)
add_subdirectory(2.LOG)
add_subdirectory(3.TCP)
add_subdirectory(5.Service)
