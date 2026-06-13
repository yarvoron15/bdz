add_executable(test_next_even
    test.cpp
    next_even.cpp
    next_even.h
)

target_link_libraries(test_next_even PRIVATE XiCatch2WithMain)
