#include <catch2/catch_shim.hpp>

#include "scorer.h"

TEST_CASE("GetScoredStudents") {
    Events events{
        {.studentName = "Ivanov", .taskName = "utf8", .time = 1600000001, .eventType = EventType::checkSuccess},
        {.studentName = "Ivanov", .taskName = "scorer", .time = 1600000002, .eventType = EventType::checkSuccess},
        {.studentName = "Petrov", .taskName = "utf8", .time = 1600000003, .eventType = EventType::checkSuccess},
        {.studentName = "Petrov", .taskName = "scorer", .time = 1600000004, .eventType = EventType::checkFailed},
        {.studentName = "Sidorov", .taskName = "scorer", .time = 1600000005, .eventType = EventType::checkFailed},
    };

    ScoreTable expectedResult = {
        {"Ivanov", {"utf8", "scorer"}},
        {"Petrov", {"utf8"}},
    };

    REQUIRE(expectedResult == getScoredStudents(events, 1600000010));
}
