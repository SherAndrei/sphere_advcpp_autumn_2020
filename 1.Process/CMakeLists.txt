cmake_minimum_required(VERSION 3.10)
project(Process)

set(CMAKE_CXX_STANDARD 17)

set(SRCS main.cpp descripter.cpp process.cpp)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format-Werror -Wpedantic)

add_executable(main ${SRCS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

foreach(test_bin_name 1test 2test 3test)
	add_executable(${test_bin_name} tests/${test_bin_name}.cpp)
endforeach(test_bin_name)
