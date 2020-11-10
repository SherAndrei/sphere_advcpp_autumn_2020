set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

foreach(test testServer testClient)
    add_executable(${test} ./${test}.cpp)
    target_link_libraries(${test} tcp)
endforeach(test)
