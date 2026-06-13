#include "admission.h"

#include <algorithm>

namespace {

bool IsBirthDateLess(const Date& lhs, const Date& rhs) {
    if (lhs.year != rhs.year) {
        return lhs.year < rhs.year;
    }
    if (lhs.month != rhs.month) {
        return lhs.month < rhs.month;
    }
    return lhs.day < rhs.day;
}

bool IsStudentLess(const Student* lhs, const Student* rhs) {
    if (lhs->name != rhs->name) {
        return lhs->name < rhs->name;
    }
    return IsBirthDateLess(lhs->birthDate, rhs->birthDate);
}

bool IsApplicantHigherPriority(const Applicant* lhs, const Applicant* rhs) {
    if (lhs->points != rhs->points) {
        return lhs->points > rhs->points;
    }
    if (IsBirthDateLess(lhs->student.birthDate, rhs->student.birthDate)) {
        return true;
    }
    if (IsBirthDateLess(rhs->student.birthDate, lhs->student.birthDate)) {
        return false;
    }
    return lhs->student.name < rhs->student.name;
}

}  // namespace

AdmissionTable fillUniversities(const std::vector<University>& universities,
                                const std::vector<Applicant>& applicants)
{
    AdmissionTable result;
    std::unordered_map<std::string, size_t> remaining_slots;

    for (const auto& university : universities) {
        remaining_slots[university.name] = university.maxStudents;
    }

    std::vector<const Applicant*> ordered_applicants;
    ordered_applicants.reserve(applicants.size());
    for (const auto& applicant : applicants) {
        ordered_applicants.push_back(&applicant);
    }

    std::sort(ordered_applicants.begin(), ordered_applicants.end(), IsApplicantHigherPriority);

    for (const Applicant* applicant : ordered_applicants) {
        for (const std::string& university_name : applicant->wishList) {
            auto it = remaining_slots.find(university_name);
            if (it != remaining_slots.end() && it->second > 0) {
                result[university_name].push_back(&applicant->student);
                --it->second;
                break;
            }
        }
    }

    for (auto& [_, students] : result) {
        std::sort(students.begin(), students.end(), IsStudentLess);
    }

    return result;
}
