#ifndef DATETIME_UTIL_HPP
#define DATETIME_UTIL_HPP

#include <sys/time.h>

#include <chrono>
#include <ctime>
#include <string>

namespace app_common {
    class DatetimeUtil {
    public:
        DatetimeUtil() = delete;

    public:
        [[maybe_unused]] static void set_time(std::time_t t) {
            timeval tv{.tv_sec = t, .tv_usec = 0};

            settimeofday(&tv, nullptr);
        }

        [[maybe_unused]] static std::time_t current_second() {
            return std::time(nullptr);
        }

        [[maybe_unused]] static std::uint64_t current_ms() {
            timeval tv{};
            gettimeofday(&tv, nullptr);
            return ((uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
        }

        [[maybe_unused]] static double current_second_double() {
            timeval tv{};
            gettimeofday(&tv, nullptr);
            return tv.tv_sec + tv.tv_usec / 1000000.0;
        }

        template <typename T>[[maybe_unused]] static double double_time_from_sec_nsec(const T& t) {
            return t.sec + t.nsec / 1e9;
        }

        template <typename T>[[maybe_unused]] static double double_time_from_tv_sec_usec(const T& t) {
            return t.tv_sec + t.tv_usec / 1e6;
        }

        template <typename T>[[maybe_unused]] static T current_sec_nsec() {
            T t{};
            // 设置时间戳：将double秒转换为{sec, nsec}格式
            double   current_time = current_second_double();
            uint32_t sec_part = static_cast<uint32_t>(current_time);
            uint32_t nsec_part = static_cast<uint32_t>((current_time - sec_part) * 1e9);
            t.sec = sec_part;
            t.nsec = nsec_part;
            return t;
        }

        [[maybe_unused]] static int32_t zone_offset_by_minute() {
            int32_t    time_zone = 0;
            time_t     t1, t2;
            struct tm *tm_local, *tm_utc;

            time(&t1);
            t2 = t1;

            tm_local = localtime(&t1);
            t1 = mktime(tm_local);
            tm_utc = gmtime(&t2);
            t2 = mktime(tm_utc);

            time_zone = (int32_t)((t1 - t2) / 60);
            return time_zone;
        }

        [[maybe_unused]] static std::tm current_tm() {
            auto    seconds = std::time(nullptr);
            std::tm t{};
            localtime_r(&seconds, &t);
            return t;
        }

        [[maybe_unused]] static std::time_t current_millisecond() {
            auto p = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
            return p.time_since_epoch().count();
        }

        [[maybe_unused]] static std::int64_t current_microsecond() {
            timeval tv{};
            gettimeofday(&tv, nullptr);
            return tv.tv_sec * 1000000 + tv.tv_usec;
        }

        [[maybe_unused]] static std::time_t ndays_off(std::time_t t, std::int32_t n) {
            std::tm tm{};
            localtime_r(&t, &tm);
            tm.tm_mday += n;
            return mktime(&tm);
        }

        // t2 - t1
        [[maybe_unused]] static int32_t diff_day(std::time_t t1, std::time_t t2) {
            std::tm tm1{}, tm2{};

            localtime_r(&t1, &tm1);
            localtime_r(&t2, &tm2);

            return diff_day(tm1, tm2);
        }

        // t2 - t1
        [[maybe_unused]] static int32_t diff_day(const std::tm& t1, const std::tm& t2) {
#if 0
        auto d1 = boost::gregorian::date_from_tm(t1);
        auto d2 = boost::gregorian::date_from_tm(t2);

        auto diff = d2 - d1;
        return diff.days();
#else
            return 0;
#endif
        }

        // is t1 = t2 + n
        [[maybe_unused]] static bool is_nhour_off(std::time_t t1, std::time_t t2, std::int32_t n) {
            std::tm tm1{}, tm2{};

            localtime_r(&t1, &tm1);
            localtime_r(&t2, &tm2);

            tm2.tm_hour += n;
            mktime(&tm2);

            return tm1.tm_year == tm2.tm_year && tm1.tm_mon == tm2.tm_mon && tm1.tm_mday == tm2.tm_mday &&
                   tm1.tm_hour == tm2.tm_hour;
        }

        [[maybe_unused]] static bool is_same_hour(std::time_t t1, std::time_t t2) {
            std::tm tm1{}, tm2{};
            localtime_r(&t1, &tm1);
            localtime_r(&t2, &tm2);
            return tm1.tm_year == tm2.tm_year && tm1.tm_mon == tm2.tm_mon && tm1.tm_mday == tm2.tm_mday &&
                   tm1.tm_hour == tm2.tm_hour;
        }

        [[maybe_unused]] static std::time_t trunc_to_day(std::time_t t) {
            std::tm tm{};
            localtime_r(&t, &tm);
            tm.tm_hour = 0;
            tm.tm_min = 0;
            tm.tm_sec = 0;
            return mktime(&tm);
        };

        [[maybe_unused]] static std::time_t trunc_to_hour(std::time_t t) {
            std::tm tm{};
            localtime_r(&t, &tm);
            tm.tm_min = 0;
            tm.tm_sec = 0;
            return mktime(&tm);
        }

        [[maybe_unused]] static std::int32_t hour(std::time_t t) {
            std::tm tm{};
            localtime_r(&t, &tm);
            return tm.tm_hour;
        }
    };
}; // namespace app_common

#endif // ZEUS_SHMS_INTF_DATETIMEUTIL_HPP
