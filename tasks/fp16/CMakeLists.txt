add_executable(test_fp16
    test.cpp
    fp16.cpp
    fp16.h
)

target_link_libraries(test_fp16 PRIVATE XiCatch2WithMain)
