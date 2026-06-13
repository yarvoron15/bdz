#include "palindrome.h"

bool isPalindrome(const std::string& str)
{
    size_t left = 0;
    size_t right = str.size();

    while (left < right) {
        while (left < right && str[left] == ' ') {
            ++left;
        }
        while (left < right && str[right - 1] == ' ') {
            --right;
        }
        if (left < right && str[left] != str[right - 1]) {
            return false;
        }
        if (left < right) {
            ++left;
            --right;
        }
    }

    return true;
}
