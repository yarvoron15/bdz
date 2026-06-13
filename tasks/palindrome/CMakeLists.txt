add_executable(test_palindrome
    test.cpp
    palindrome.cpp
    palindrome.h
)

target_link_libraries(test_palindrome PRIVATE XiCatch2WithMain)
