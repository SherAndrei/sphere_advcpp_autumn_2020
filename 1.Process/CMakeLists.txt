cmake_minimum_required(VERSION 3.10)
project(Process)

set(CMAKE_CXX_STANDARD 17)

set(HDR hdr)
set(SRC src)
set(TESTDIR tests)

include_directories( ${HDR} )
FILE(GLOB SRCS ${SRC}/*.cpp)

add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format-Werror -Wpedantic)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
add_executable(main ${TESTDIR}/main.cpp ${SRCS})

foreach(test_name 1test 2test 3test)
	add_executable(${test_name} ${TESTDIR}/${test_name}.cpp)
endforeach(test_name)
