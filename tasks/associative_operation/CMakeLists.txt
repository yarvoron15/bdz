add_executable(test_associative_operation
    test.cpp
    associative_operation.cpp
    associative_operation.h
)

target_link_libraries(test_associative_operation PRIVATE XiCatch2WithMain)
