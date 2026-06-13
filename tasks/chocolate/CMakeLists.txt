add_executable(test_chocolate
    test.cpp
    chocolate.cpp
    chocolate.h
)

target_link_libraries(test_chocolate PRIVATE XiCatch2WithMain)
