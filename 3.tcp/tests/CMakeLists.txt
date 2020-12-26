foreach(test testServer testClient)
    add_executable(${test} ./${test}.cpp)
    target_link_libraries(${test} tcp)
endforeach(test)
