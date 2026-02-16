#ifndef TESTS_CHECKING_H_
#define TESTS_CHECKING_H_

#include <string>
#include <vector>

enum class Side {
    top,
    bottom,
};

struct StudentAction {
    std::string name;
    Side side = Side::top;
};

std::vector<std::string> studentsOrder(const std::vector<StudentAction>& studentActions,
                                       const std::vector<size_t>& queries);

#endif // TESTS_CHECKING_H_
