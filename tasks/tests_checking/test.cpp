#include <catch2/catch_shim.hpp>

#include "tests_checking.h"

TEST_CASE("StudentsOrder")
{
    const std::vector<StudentAction> studentsActions =
    {
        {.name = "Ivanov", .side = Side::top},
        {.name = "Petrov", .side = Side::top},
        {.name = "Sidorov", .side = Side::bottom},
    };
    const std::vector<size_t> queries = {1, 3};
    const std::vector<std::string> expected = {"Petrov", "Sidorov"};

    REQUIRE(expected == studentsOrder(studentsActions, queries));
}
