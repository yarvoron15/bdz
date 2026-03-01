add_executable(test_tests_checking
    test.cpp
    tests_checking.cpp
    tests_checking.h
)

target_link_libraries(test_tests_checking PRIVATE XiCatch2WithMain)
