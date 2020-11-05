cmake_minimum_required(VERSION 3.10)
project(TCP)

set(CMAKE_CXX_STANDARD 17)

set(HDR include)
set(LIB lib)
set(TESTS tests)

set(LIB_NAME tcp)
add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format -Wpedantic)

add_library(${LIB_NAME} STATIC ${LIB}/connection.cpp ${LIB}/address.cpp ${LIB}/descripter.cpp ${LIB}/error.cpp ${LIB}/server.cpp)
target_include_directories(${LIB_NAME} PUBLIC include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../${TESTS})

foreach(test testServer testClient)
    add_executable(${test} ${TESTS}/${test}.cpp)
    target_link_libraries(${test} ${LIB_NAME})
endforeach(test)