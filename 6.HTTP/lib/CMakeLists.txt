
add_library(http STATIC message.cpp
                        status_code.cpp
                        httpconnection.cpp
                        httpservice.cpp
                        iTimed.cpp)
target_include_directories(http PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(http PUBLIC ${CMAKE_SOURCE_DIR}/2.LOG/include)
target_include_directories(http PUBLIC ${CMAKE_SOURCE_DIR}/3.TCP/include)
target_include_directories(http PUBLIC ${CMAKE_SOURCE_DIR}/5.TCP/include)
target_link_libraries(http pthread)
target_link_libraries(http log)
target_link_libraries(http tcp)
target_link_libraries(http net)
