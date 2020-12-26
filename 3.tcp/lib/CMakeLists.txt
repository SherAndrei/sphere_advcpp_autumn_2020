add_library(tcp STATIC connection.cpp
                       nonblock_connection.cpp
                       address.cpp
                       descriptor.cpp
                       socket.cpp
                       tcperr.cpp
                       server.cpp)

target_include_directories(tcp PUBLIC ${PROJECT_SOURCE_DIR}/include)
