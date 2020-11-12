cmake_minimum_required(VERSION 3.10)
project(LOG)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format -Wpedantic -Wmissing-declarations)

add_subdirectory(tests)
add_subdirectory(lib)
