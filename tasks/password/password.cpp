#include "password.h"

#include <cctype>

bool validatePassword(const std::string& password)
{
    if (password.size() < 8 || password.size() > 14) {
        return false;
    }

    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;
    bool has_other = false;

    for (unsigned char ch : password) {
        if (ch < 33 || ch > 126) {
            return false;
        }
        if (std::isupper(ch) != 0) {
            has_upper = true;
        } else if (std::islower(ch) != 0) {
            has_lower = true;
        } else if (std::isdigit(ch) != 0) {
            has_digit = true;
        } else {
            has_other = true;
        }
    }

    return static_cast<int>(has_upper) + static_cast<int>(has_lower) +
               static_cast<int>(has_digit) + static_cast<int>(has_other) >=
           3;
}
