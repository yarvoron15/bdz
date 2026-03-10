add_executable(test_vector_minmax
    test.cpp
    vector_minmax.cpp
    vector_minmax.h
)

target_link_libraries(test_vector_minmax PRIVATE XiCatch2WithMain)
