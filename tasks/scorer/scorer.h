#ifndef SCORER_H_
#define SCORER_H_


#include <map>
#include <set>
#include <string>
#include <vector>

enum class EventType {
    checkFailed,
    checkSuccess,
    mergeRequestOpen,
    mergeRequestClosed,
};

using StudentName = std::string;
using TaskName = std::string;

struct Event {
    StudentName studentName;
    TaskName taskName;
    time_t time;
    EventType eventType;
};

using Events = std::vector<Event>;
using ScoreTable = std::map<StudentName, std::set<TaskName>>;

ScoreTable getScoredStudents(const Events& events, time_t scoreTime);


#endif // SCORER_H_
