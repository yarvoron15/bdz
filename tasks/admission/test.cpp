#include <catch2/catch_shim.hpp>

#include "admission.h"

TEST_CASE("FillUniversities") {
    const std::vector<University> universities = {
        {.name = "MSU", .maxStudents = 1},
        {.name = "HSE", .maxStudents = 2},
        {.name = "MIPT", .maxStudents = 100}
    };

    const std::vector<Applicant> applicants = {
        Applicant{.student = {.name = "Ivan Ivanov", .birthDate = {.day = 1, .month = 1, .year = 1900}},
                  .points = 100,
                  .wishList = {"MSU", "HSE"}},
        Applicant{.student = {.name = "Petr Petrov", .birthDate = {.day = 2, .month = 1, .year = 1900}},
                  .points = 90,
                  .wishList = {"MSU", "HSE"}},
        Applicant{.student = {.name = "Alexander Sidorov", .birthDate = {.day = 3, .month = 1, .year = 1900}},
                  .points = 110,
                  .wishList = {"MIPT", "HSE"}},
        Applicant{.student = {.name = "Ivan Petrov", .birthDate = {.day = 3, .month = 1, .year = 1900}},
                  .points = 100,
                  .wishList = {"HSE", "MSU", "MIPT"}},
        Applicant{.student = {.name = "Petr Ivanov", .birthDate = {.day = 4, .month = 1, .year = 1900}},
                  .points = 80,
                  .wishList = {"HSE"}}};

    const AdmissionTable expected = {{"HSE", {&applicants[3].student, &applicants[1].student}},
                                     {"MIPT", {&applicants[2].student}},
                                     {"MSU", {&applicants[0].student}}};

    REQUIRE(expected == fillUniversities(universities, applicants));
}
