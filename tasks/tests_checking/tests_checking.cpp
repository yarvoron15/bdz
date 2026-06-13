#include "tests_checking.h"

#include <deque>

std::vector<std::string> studentsOrder(const std::vector<StudentAction>& studentActions,
                                       const std::vector<size_t>& queries)
{
    std::deque<std::string> works;
    for (const auto& action : studentActions) {
        if (action.side == Side::top) {
            works.push_front(action.name);
        } else {
            works.push_back(action.name);
        }
    }

    std::vector<std::string> result;
    result.reserve(queries.size());
    for (size_t query : queries) {
        result.push_back(works[query - 1]);
    }
    return result;
}
