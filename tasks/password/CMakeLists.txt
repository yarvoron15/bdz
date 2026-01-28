add_executable(test_password
    test.cpp
    password.cpp
    password.h
)

target_link_libraries(test_password PRIVATE XiCatch2WithMain)
