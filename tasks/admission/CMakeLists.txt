add_executable(test_admission
    test.cpp
    admission.cpp
    admission.h
)

target_link_libraries(test_admission PRIVATE XiCatch2WithMain)
