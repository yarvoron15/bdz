add_executable(test_varint
    test.cpp
    varint.cpp
    varint.h
)

target_link_libraries(test_varint PRIVATE XiCatch2WithMain)
