add_executable(test_safe_arithmetic
    test.cpp
    safe_arithmetic.cpp
    safe_arithmetic.h
)

target_link_libraries(test_safe_arithmetic PRIVATE XiCatch2WithMain)
