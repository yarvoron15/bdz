#include "sort_students.h"

#include <algorithm>

namespace {

bool DateLess(const Date& lhs, const Date& rhs) {
    if (lhs.year != rhs.year) {
        return lhs.year < rhs.year;
    }
    if (lhs.month != rhs.month) {
        return lhs.month < rhs.month;
    }
    return lhs.day < rhs.day;
}

}  // namespace

void sortStudents(std::vector<Student>& students, SortKind sortKind)
{
    std::sort(students.begin(), students.end(), [sortKind](const Student& lhs, const Student& rhs) {
        if (sortKind == SortKind::date) {
            if (DateLess(lhs.birthDate, rhs.birthDate)) {
                return true;
            }
            if (DateLess(rhs.birthDate, lhs.birthDate)) {
                return false;
            }
            return std::tie(lhs.lastName, lhs.name) < std::tie(rhs.lastName, rhs.name);
        }

        if (std::tie(lhs.lastName, lhs.name) != std::tie(rhs.lastName, rhs.name)) {
            return std::tie(lhs.lastName, lhs.name) < std::tie(rhs.lastName, rhs.name);
        }
        return DateLess(lhs.birthDate, rhs.birthDate);
    });
}
