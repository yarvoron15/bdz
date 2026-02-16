#ifndef SORT_STUDENTS_H_
#define SORT_STUDENTS_H_


#include <vector>
#include <string>

struct Date {
    int day = 0;
    int month = 0;
    int year = 0;
};

struct Student {
    std::string name;
    std::string lastName;
    Date birthDate;
};

enum class SortKind {
    name,
    date
};

void sortStudents(std::vector<Student>& students, SortKind sortKind);


#endif // SORT_STUDENTS_H_
