add_executable(test_scorer
    test.cpp
    scorer.cpp
    scorer.h
)

target_link_libraries(test_scorer PRIVATE XiCatch2WithMain)
