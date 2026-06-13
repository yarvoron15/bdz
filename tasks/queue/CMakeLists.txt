add_executable(test_queue
    test.cpp
    queue.cpp
    queue.h
)

target_link_libraries(test_queue PRIVATE XiCatch2WithMain)
