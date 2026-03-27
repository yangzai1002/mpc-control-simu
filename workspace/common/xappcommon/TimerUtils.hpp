#ifndef COMMON_UTILS_TIMER_UTILS_HPP
#define COMMON_UTILS_TIMER_UTILS_HPP

#include <sys/time.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

namespace app_common {
    class TimerUtils {
    public:
        static TimerUtils& get() {
            static TimerUtils inst;
            inst.start_loop();
            return inst;
        }

        static std::uint32_t create(std::uint32_t ms, std::function<void()>&& v) {
            return get().add_timer(ms, std::move(v));
        }

        static std::uint32_t create(std::uint32_t ms, std::function<void()>&& v, std::uint32_t limit) {
            return get().add_timer(ms, std::move(v), limit);
        }

        static void drop(std::uint32_t& timer_id) {
            get().del_timer(timer_id);
            timer_id = 0;
        }

    private:
        TimerUtils() : stopped(false), id(time(nullptr)) {
            start_loop();
        };

        ~TimerUtils() {
            stop_loop();
        }

        std::uint32_t add_timer(std::uint32_t ms, std::function<void()>&& v) {
            std::lock_guard<std::mutex> guard(mtx);

            auto timer_id = id++;
            id_item_map.emplace(timer_id, Item(timer_id, ms * 1000, std::move(v)));
            return timer_id;
        }

        std::uint32_t add_timer(std::uint32_t ms, std::function<void()>&& v, std::uint32_t limit) {
            std::lock_guard<std::mutex> guard(mtx);
            auto                        timer_id = id++;
            id_item_map.emplace(timer_id, Item(timer_id, ms * 1000, std::move(v), limit));
            return timer_id;
        }

        void del_timer(std::uint32_t timer_id) {
            std::lock_guard<std::mutex> guard(mtx);
            id_item_map.erase(timer_id);
        }

        void start_loop() {
            thread_timing = std::thread([this]() { loop(); });

            thread_exec = std::thread([this]() { exec(); });
        }

        void stop_loop() {
            stopped = true;

            if (thread_timing.joinable()) {
                thread_timing.join();
            }

            if (thread_exec.joinable()) {
                thread_exec.join();
            }
        }

        void loop() {
            std::uint32_t sleep = UNIT_US;
            timeval       start{};
            timeval       end{};

            while (!stopped) {
                usleep(sleep);

                gettimeofday(&start, nullptr);
                proc();
                gettimeofday(&end, nullptr);

                auto minus = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
                if (minus > 0) {
                    if (UNIT_US > minus) {
                        sleep = UNIT_US - minus;
                    } else {
                        sleep = 0;
                    }
                }
            }
        }

        void proc() {
            std::lock_guard<std::mutex> guard(mtx);

            for (auto iter = id_item_map.begin(); iter != id_item_map.end();) {
                auto& item = iter->second;
                item.passed_us += UNIT_US;

                if (item.passed_us >= item.us) {
                    {
                        std::lock_guard<std::mutex> guard1(exec_mutex);
                        execs.emplace_back(item.exec);
                        if (1 == execs.size()) {
                            exec_cond.notify_all();
                        }
                    }

                    item.passed_us = 0;
                    ++item.e_count;
                    if (0 != item.count && item.e_count >= item.count) {
                        iter = id_item_map.erase(iter);
                    } else {
                        ++iter;
                    }
                } else {
                    ++iter;
                }
            }
        }

        void exec() {
            while (!stopped) {
                std::optional<std::function<void()>> to_exec;

                // 先取出来再执行，以免在 Timer 的回调函数中调用 Timer 的
                // API，从而导致间接引起死锁
                {
                    std::unique_lock<std::mutex> lk(exec_mutex);
                    if (execs.empty()) {
                        exec_cond.wait_for(lk, std::chrono::milliseconds(50));
                    }
                    if (!execs.empty() && !stopped) {
                        to_exec = std::move(execs.front());
                        execs.pop_front();
                    }
                }

                if (to_exec.has_value()) {
                    to_exec.value()();
                    to_exec.reset();
                }
            }
        }

    private:
        volatile std::atomic_bool stopped;

        const std::uint32_t UNIT_US = 5000;

        struct Item {
            const std::uint32_t         id;
            const std::uint32_t         us;
            const std::uint32_t         count;
            const std::function<void()> exec;
            std::uint32_t               passed_us;
            std::uint32_t               e_count;

            Item(std::uint32_t id, std::uint32_t us, std::function<void()>&& e, std::uint32_t count = 0)
                : id(id), us(us), count(count), exec(std::move(e)), passed_us(0), e_count(0) {
            }
        };

    private:
        std::uint32_t                 id;
        std::mutex                    mtx;
        std::map<std::uint32_t, Item> id_item_map;

        std::mutex                       exec_mutex;
        std::condition_variable          exec_cond;
        std::list<std::function<void()>> execs;

        std::thread thread_timing;
        std::thread thread_exec;
    };
};     // namespace app_common
#endif // HELMET_MCU_TIMERUTIL_HPP
