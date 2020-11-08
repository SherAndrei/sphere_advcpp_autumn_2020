
add_library(net STATIC bufconnection.cpp
                       epoll.cpp
                       listener.cpp
                       service.cpp
                       neterr.cpp)
target_include_directories(net PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(net PUBLIC ${CMAKE_SOURCE_DIR}/3.TCP/include)
target_link_libraries(net tcp)
