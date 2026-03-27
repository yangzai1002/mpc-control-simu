#ifndef BASE_MALFUNCTION_HPP
#define BASE_MALFUNCTION_HPP

#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <utility>

class Malfunction {
public:
    enum class MalType : std::int32_t {
        MALTYPE_MEDIAN_MAN_LIGTH,
        MALTYPE_MEDIAN_MAN_IR,
        MALTYPE_RECORDER_LIGHT,
        MALTYPE_RECORDER_IR,
        MALTYPE_RTSP,
        MALTYPE_28281,
        MALTYPE_HW_VENC,
        MALTYPE_HW_LIGHT,
        MALTYPE_HW_IR,
        MALTYPE_HW_SD,
        MALTYPE_HW_TEMPERATURE,   // 未检测到温度传感器
        MALTYPE_OVERTEMPERATURE,  // 温度过高
        MALTYPE_UNDERTEMPERATURE, // 温度过低
    };

    struct MalEntry {
        MalType     _type;
        std::string _reason;
        MalEntry(MalType type, std::string reason) : _type(type), _reason(std::move(reason)) {
        }
    };

    void occur(MalType type, std::string reason) noexcept(true) {
        std::lock_guard<std::mutex> g(_mutex);

        _malfunctions.emplace(type, std::move(reason));
    }

    std::list<MalEntry> malfunctions() {
        std::lock_guard<std::mutex> g(_mutex);

        std::list<Malfunction::MalEntry> l;
        for (const auto &x : _malfunctions) {
            if (!x.second.empty()) {
                l.emplace_back(MalEntry{x.first, x.second});
            }
        }

        return std::move(l);
    }

private:
    std::mutex                     _mutex;
    std::map<MalType, std::string> _malfunctions;
};

#endif
