foreach(test parsertest httpserver httpclient)
    add_executable(${test} ${test}.cpp )
    target_link_libraries(${test} http)    
endforeach(test)


