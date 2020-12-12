
add_library(cor STATIC coroutine.cpp
                       corconnection.cpp
                       corworker.cpp
                       corservice.cpp)
target_include_directories(cor PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(cor PUBLIC ${CMAKE_SOURCE_DIR}/2.LOG/include)
target_include_directories(cor PUBLIC ${CMAKE_SOURCE_DIR}/3.TCP/include)
target_include_directories(cor PUBLIC ${CMAKE_SOURCE_DIR}/5.TCP/include)
target_include_directories(cor PUBLIC ${CMAKE_SOURCE_DIR}/6.HTTP/include)
target_link_libraries(cor pthread)
target_link_libraries(cor log)
target_link_libraries(cor tcp)
target_link_libraries(cor net)
target_link_libraries(cor http)
