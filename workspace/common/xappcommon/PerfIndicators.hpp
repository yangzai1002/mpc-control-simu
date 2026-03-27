#ifndef PERF_INDICATOR_HPP
#define PERF_INDICATOR_HPP

#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <list>

#include "log/log.h"

namespace app_common {

    template <std::size_t SAMPLE_DURATION_MS, std::size_t SAMPLE_COUNT> struct PerSecondIndicator {
    private:
        struct Slot {
            std::int64_t ms;
            double       packets;
            double       bytes;

            Slot() {
                reset();
            }

            Slot(std::int64_t ms, double packets, double bytes) {
                this->ms = ms;
                this->packets = packets;
                this->bytes = bytes;
            }

            void reset() {
                ms = 0;
                packets = 0;
                bytes = 0;
            }

            void reset(std::int16_t ms) {
                this->ms = ms;
                packets = 0;
                bytes = 0;
            }
        };

    public:
        static_assert(0 == 1000 % SAMPLE_DURATION_MS && 0 == SAMPLE_COUNT % SAMPLE_DURATION_MS && SAMPLE_COUNT > 0,
                      "ERROR");

        PerSecondIndicator() {
            reset();
        }

        void reset() {
            _sum_packets = 0;
            _sum_bytes = 0;
            _slots.clear();
        }

        void add(std::int64_t ms, std::uint32_t sum_packets, std::uint32_t sum_bytes) {
            if (_slots.size() > SAMPLE_COUNT) {
                const auto &slot = _slots.front();
                _sum_packets -= slot.packets;
                _sum_bytes -= slot.bytes;
                _slots.pop_front();
            }

            if (!_slots.empty() && (abs(_slots.back().ms - ms) < SAMPLE_DURATION_MS)) {
                auto &slot = _slots.back();
                slot.packets += sum_packets;
                slot.bytes += sum_bytes;
            } else {
                _slots.emplace_back(ms, sum_packets, sum_bytes);
            }
            _sum_packets += sum_packets;
            _sum_bytes += sum_bytes;
        }

        std::pair<double, double> avg_per_second() {
            if (_slots.size() < 2) {
                return {0, 0};
            }

            auto span = abs(_slots.back().ms - _slots.front().ms) / 1000.0;
            return {_sum_packets / span, _sum_bytes / span};
        }

    private:
        std::list<Slot> _slots;
        double          _sum_packets;
        double          _sum_bytes;
    };

    template <std::size_t SAMPLE_COUNT> struct TimeCostIndicator {
        TimeCostIndicator() : _sum_costs(0.0) {
            reset();
        }

        void reset() {
            _sum_costs = 0;
            _costs.clear();
        }

        void add(double cost_in_us) {
            if (_costs.size() >= SAMPLE_COUNT) {
                _sum_costs -= _costs.front();
                _costs.pop_front();
            }

            _costs.push_back(cost_in_us);
            _sum_costs += cost_in_us;
        }

        double avg() {
            if (_costs.empty()) {
                return 0;
            }
            return _sum_costs / _costs.size();
        }

    private:
        double            _sum_costs;
        std::list<double> _costs;
    };

    template <std::size_t SAMPLE_COUNT, typename T> struct FixedSizeIndicator {
        FixedSizeIndicator() {
            reset();
        }

        void reset() {
            _values.clear();
        }

        void add(T v) {
            if (_values.size() >= SAMPLE_COUNT) {
                _values.pop_front();
            }

            _values.push_back(v);
        }

        bool is_all(std::function<bool(const T &)> pred) {
            if (_values.size() < SAMPLE_COUNT) {
                return false;
            }
            for (const auto &v : _values) {
                if (!pred(v)) {
                    return false;
                }
            }
            return true;
        }

    private:
        std::list<T> _values;
    };

    struct CounterIndicator {
        CounterIndicator() : _n(0) {
        }

        void reset() {
            _n = 0;
        }

        void add(std::uint32_t n) {
            _n += n;
        }

        std::uint32_t n() {
            return _n;
        }

    private:
        std::uint32_t _n;
    };

    template <typename T> class TimeWindowIndicator {
    public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = Clock::duration;

        explicit TimeWindowIndicator(Duration window) : _window(window) {
        }

        void reset() {
            _items.clear();
        }

        void add(const T &v) {
            add(Clock::now(), v);
        }

        void add(TimePoint tp, const T &v) {
            _items.emplace_back(tp, v);
            cleanup(tp);
        }

        std::pair<TimePoint, TimePoint> time_range() {
            if (_items.empty()) {
                return std::make_pair(TimePoint(), TimePoint());
            }

            return std::make_pair(_items.front().tp, _items.back().tp);
        }

        bool is_all(std::function<bool(const T &)> pred) {
            auto now = Clock::now();
            cleanup(now);

            if (_items.size() < 2) {
                return false;
            }
            auto b = _items.front().tp;
            auto e = _items.back().tp;
            if ((e - b) < _window) {
                return false;
            }

            for (auto &it : _items) {
                if (!pred(it.value)) {
                    return false;
                }
            }
            return true;
        }

        std::size_t size() const {
            return _items.size();
        }

    private:
        struct Item {
            TimePoint tp;
            T         value;
            Item(TimePoint t, const T &v) : tp(t), value(v) {
            }
        };

        void cleanup(TimePoint now) {
            while (!_items.empty()) {
                auto &front = _items.front();
                // 预留一定的数据量，以避免 is_all 判断的时候总是返回 false。
                if ((now - front.tp) / 1.2 > _window) {
                    _items.pop_front();
                } else {
                    break;
                }
            }
        }

    private:
        Duration         _window;
        std::deque<Item> _items;
    };

} // namespace app_common

#endif
