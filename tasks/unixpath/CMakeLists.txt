add_executable(test_unixpath
    test.cpp
    unixpath.cpp
    unixpath.h
)

target_link_libraries(test_unixpath PRIVATE XiCatch2WithMain)
