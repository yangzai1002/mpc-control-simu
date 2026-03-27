#ifndef _PLANNING_FEEDBACK_
#define _PLANNING_FEEDBACK_
#include <string>
#include <vector>

namespace avos {
namespace planning {

struct PlanningStatus {
    PlanningStatus() {
        Reset();
    }

    void Reset() {
        status.clear();
        warning = 0;
    }

    void SetStatus(const std::string& str) {
        status.emplace_back(str);
    }

    std::vector<std::string> GetStatus() const {
        return status;
    }

    void SetWarning(const int& w) {
        warning = w;
    }

    int GetWarning() const {
        return warning;
    }

private:
    std::vector<std::string> status;
    int warning;
};

} // namespace planning
} // namespace avos

#endif // _PLANNING_FEEDBACK_