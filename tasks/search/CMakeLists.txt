add_executable(test_search
    test.cpp
    search.cpp
    search.h
)

target_link_libraries(test_search PRIVATE XiCatch2WithMain)
