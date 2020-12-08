foreach(test server simple keep_alive keep_alive_zombie zombie parsertest)
    add_executable(${test} ${test}.cpp )
    target_link_libraries(${test} http)
endforeach(test)
