set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_executable(test test.cpp)
target_link_libraries(test shmem)