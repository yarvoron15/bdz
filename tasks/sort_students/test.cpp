#include <catch2/catch_shim.hpp>

#include "sort_students.h"

namespace {

struct TestData {
    std::vector<Student> students;
    SortKind sortKind = SortKind::name;
    std::vector<Student> result;
};

const std::vector<TestData> TESTS_DATA = {
    {
        .students = {
            { .name = "Petr", .lastName = "Petrov", .birthDate = { .day = 2, .month = 2, .year = 1902 }},
            { .name = "Ivan", .lastName = "Ivanov", .birthDate = { .day = 1, .month = 1, .year = 1901 }},
            { .name = "Alexander", .lastName = "Sidorov", .birthDate = { .day = 3, .month = 3, .year = 1903 }}
        },
        .sortKind = SortKind::date,
        .result = {
            { .name = "Ivan", .lastName = "Ivanov", .birthDate = { .day = 1, .month = 1, .year = 1901 }},
            { .name = "Petr", .lastName = "Petrov", .birthDate = { .day = 2, .month = 2, .year = 1902 }},
            { .name = "Alexander", .lastName = "Sidorov", .birthDate = { .day = 3, .month = 3, .year = 1903 }}
        }
    },
    {
        .students = {
            { .name = "Petr", .lastName = "Petrov", .birthDate = { .day = 2, .month = 2, .year = 1902 }},
            { .name = "Alexander", .lastName = "Sidorov", .birthDate = { .day = 3, .month = 3, .year = 1903 }},
            { .name = "Ivan", .lastName = "Ivanov", .birthDate = { .day = 1, .month = 1, .year = 1901 }}
        },
        .sortKind = SortKind::name,
        .result = {
            { .name = "Ivan", .lastName = "Ivanov", .birthDate = { .day = 1, .month = 1, .year = 1901 }},
            { .name = "Petr", .lastName = "Petrov", .birthDate = { .day = 2, .month = 2, .year = 1902 }},
            { .name = "Alexander", .lastName = "Sidorov", .birthDate = { .day = 3, .month = 3, .year = 1903 }}
        }
    },
};

bool areDatesEqual(const Date& date1, const Date& date2)
{
    return std::tie(date1.day, date1.month, date1.year) ==
           std::tie(date2.day, date2.month, date2.year);
}

bool areStudentsEqual(const Student& student1, const Student& student2)
{
    return (std::tie(student1.name, student1.lastName) ==
           std::tie(student2.name, student2.lastName)) &&
           areDatesEqual(student1.birthDate, student2.birthDate);
}

} // anonymous nmsp

TEST_CASE("SortStudents")
{
    for (TestData test : TESTS_DATA)
    {
        sortStudents(test.students, test.sortKind);
        REQUIRE(std::equal(test.students.begin(), test.students.end(),
                           test.result.begin(), test.result.end(), areStudentsEqual));
    }
}
