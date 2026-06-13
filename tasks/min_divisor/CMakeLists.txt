add_executable(test_min_divisor
    test.cpp
    min_divisor.cpp
    min_divisor.h
)

target_link_libraries(test_min_divisor PRIVATE XiCatch2WithMain)
