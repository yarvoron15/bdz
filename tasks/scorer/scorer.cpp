#include "scorer.h"

#include <algorithm>
#include <tuple>

namespace {

struct TaskState {
    bool last_check_success = false;
    bool has_check = false;
    bool merge_request_open = false;
};

}  // namespace

ScoreTable getScoredStudents(const Events& events, time_t score_time)
{
    Events filtered_events;
    filtered_events.reserve(events.size());
    for (const auto& event : events) {
        if (event.time <= score_time) {
            filtered_events.push_back(event);
        }
    }

    std::sort(filtered_events.begin(), filtered_events.end(), [](const Event& lhs, const Event& rhs) {
        return std::tie(lhs.time, lhs.studentName, lhs.taskName) <
               std::tie(rhs.time, rhs.studentName, rhs.taskName);
    });

    std::map<std::pair<StudentName, TaskName>, TaskState> states;

    for (const auto& event : filtered_events) {
        TaskState& state = states[{event.studentName, event.taskName}];
        switch (event.eventType) {
            case EventType::checkFailed:
                state.has_check = true;
                state.last_check_success = false;
                break;
            case EventType::checkSuccess:
                state.has_check = true;
                state.last_check_success = true;
                break;
            case EventType::mergeRequestOpen:
                state.merge_request_open = true;
                break;
            case EventType::mergeRequestClosed:
                state.merge_request_open = false;
                break;
        }
    }

    ScoreTable result;
    for (const auto& [key, state] : states) {
        if (state.has_check && state.last_check_success && !state.merge_request_open) {
            result[key.first].insert(key.second);
        }
    }

    return result;
}
