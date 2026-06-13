add_executable(test_provinces
    test.cpp
    provinces.cpp
    provinces.h
)

target_link_libraries(test_provinces PRIVATE XiCatch2WithMain)
