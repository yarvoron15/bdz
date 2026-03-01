#ifndef ADMISSION_H_
#define ADMISSION_H_

#include <vector>
#include <string>
#include <unordered_map>

struct Date {
    int day = 0;
    int month = 0;
    int year = 0;
};

struct Student {
    std::string name;
    Date birthDate;
};

struct University {
    std::string name;
    size_t maxStudents = 0;
};

struct Applicant {
    Student student;
    int points;
    std::vector<std::string> wishList;
};

using AdmissionTable = std::unordered_map<std::string, std::vector<const Student*>>;

AdmissionTable fillUniversities(const std::vector<University>& universities,
                                const std::vector<Applicant>& applicants);

#endif  // ADMISSION_H_
