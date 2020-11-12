
add_library(shmem STATIC shmemerr.cpp
                         semlock.cpp
                         sem.cpp
                         shmmap.cpp)

target_link_libraries(shmem pthread)
target_include_directories(shmem PUBLIC ${PROJECT_SOURCE_DIR}/include)
