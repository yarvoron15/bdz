add_executable(test_fibonacci
    test.cpp
    fibonacci.cpp
    fibonacci.h
)

target_link_libraries(test_fibonacci PRIVATE XiCatch2WithMain)
