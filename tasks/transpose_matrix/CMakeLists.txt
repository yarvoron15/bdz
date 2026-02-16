add_executable(test_transpose_matrix
    test.cpp
    transpose_matrix.cpp
    transpose_matrix.h
)

target_link_libraries(test_transpose_matrix PRIVATE XiCatch2WithMain)
