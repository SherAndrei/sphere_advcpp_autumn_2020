set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

foreach(task client calculator)
    add_executable(${task} ./${task}.cpp)
    target_link_libraries(${task} net)
endforeach(task)
