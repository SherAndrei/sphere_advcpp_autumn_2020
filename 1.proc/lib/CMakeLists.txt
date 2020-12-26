add_library(prc STATIC descriptor.cpp
                       prcerr.cpp
                       process.cpp)

target_include_directories(prc PUBLIC ${PROJECT_SOURCE_DIR}/include)
