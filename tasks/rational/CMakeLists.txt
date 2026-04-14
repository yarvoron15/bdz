add_executable(test_rational
    test.cpp
    rational.cpp
    rational.h
)

target_link_libraries(test_rational PRIVATE XiCatch2WithMain)
