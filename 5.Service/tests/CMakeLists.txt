foreach(task client calculator echo)
    add_executable(${task} ./${task}.cpp)
    target_link_libraries(${task} net)
endforeach(task)
