cmake_minimum_required(VERSION 3.10)
project(SHAREDMAP)

set(CMAKE_CXX_STANDARD 17)

set(HDR include)
set(LIB lib)
set(TESTS tests)

set(LIB_NAME shmem)
add_compile_options(-mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align 
-Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security 
-Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long  
-Wcast-qual -Wno-suggest-attribute=format -Wpedantic)
link_libraries(pthread)

add_library(${LIB_NAME} STATIC ${LIB}/sherr.cpp
                               ${LIB}/shsemlock.cpp
                               ${LIB}/shsemaphore.cpp
                               ${LIB}/shmmap.cpp)
target_include_directories(${LIB_NAME} PUBLIC ${HDR})

add_subdirectory(${TESTS})