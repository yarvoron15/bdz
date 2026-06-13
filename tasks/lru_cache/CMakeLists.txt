add_executable(test_lru_cache
    test.cpp
    lru_cache.cpp
    lru_cache.h
)

target_link_libraries(test_lru_cache PRIVATE XiCatch2WithMain)
