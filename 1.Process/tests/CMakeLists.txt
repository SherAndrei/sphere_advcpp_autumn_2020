set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
add_executable(main main.cpp )
target_link_libraries(main prc)

foreach(test_name 1test 2test 3test)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} prc)
endforeach(test_name)
