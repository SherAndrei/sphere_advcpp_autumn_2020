foreach(test server simple keep_alive)
    add_executable(${test} ${test}.cpp )
    target_link_libraries(${test} http)
endforeach(test)


