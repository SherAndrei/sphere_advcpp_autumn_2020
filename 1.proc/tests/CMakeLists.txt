link_libraries(prc)
add_executable(main main.cpp)

foreach(test_name 1test 2test 3test)
    add_executable(${test_name} ${test_name}.cpp)
endforeach(test_name)
