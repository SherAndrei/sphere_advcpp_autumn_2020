foreach(test corserver)
    add_executable(${test} ${test}.cpp )
    target_link_libraries(${test} cor)
endforeach(test)
