add_executable(test_stack
    test.cpp
    stack.cpp
    stack.h
)

target_link_libraries(test_stack PRIVATE XiCatch2WithMain)
