#ifndef _CONTROL_FLAGS_
#define _CONTROL_FLAGS_

namespace avos {
namespace planning {


enum class Stop{
    NO_STOP = 0,
    SLOW_STOP,
    PAUSE_STOP,
    EMERGENCY_STOP
};

struct ControlFlags {
    Stop stop;
    bool executing;
    int gostraight;
    ControlFlags() {
        Reset();
    }
    void Reset() {
        stop = Stop::NO_STOP;
        executing = false;
        gostraight = 0;
    }
};

} // namespace planning
} // namespace avos

#endif // _CONTROL_FLAGS_
