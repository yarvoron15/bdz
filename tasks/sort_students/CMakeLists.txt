add_executable(test_sort_students
    test.cpp
    sort_students.cpp
    sort_students.h
)

target_link_libraries(test_sort_students PRIVATE XiCatch2WithMain)

