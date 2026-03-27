//
// Created by steve on 2021/6/18.
//

#ifndef PRESAMPLER_VIBRATION_DATETIME19UTIL_HPP
#define PRESAMPLER_VIBRATION_DATETIME19UTIL_HPP

#include <ctime>
#include <sstream>
#include <string>

#include "DatetimeUtil.hpp"

namespace app_common {
    class Datetime19Util {
    public:
        Datetime19Util() = delete;

    private:
        enum Offsets {
            OFFSET_YEAR = 0,
            OFFSET_MONTH = 5,
            OFFSET_MDAY = 8,
            OFFSET_HOUR = 11,
            OFFSET_MINUTE = 14,
            OFFSET_SECOND = 17
        };

    public:
        [[maybe_unused]] static void set_time(const std::string& t) {
            DatetimeUtil::set_time(to_time_t(t));
        }

        [[maybe_unused]] static std::string current_yyyymmddhhmiss() {
            auto seconds = std::time(nullptr);
            return from_time_t(seconds);
        }

        [[maybe_unused]] static bool is_format_yyyymmddhhmiss(const std::string& t) {
            return t.size() == 19;
        }

        [[maybe_unused]] static std::time_t to_time_t(const std::string& time) {
            std::tm timeLocal = {0};

            std::stringstream(time.substr(OFFSET_YEAR, 4)) >> timeLocal.tm_year;
            timeLocal.tm_year -= 1900;
            std::stringstream(time.substr(OFFSET_MONTH, 2)) >> timeLocal.tm_mon;
            timeLocal.tm_mon -= 1;
            std::stringstream(time.substr(OFFSET_MDAY, 2)) >> timeLocal.tm_mday;
            std::stringstream(time.substr(OFFSET_HOUR, 2)) >> timeLocal.tm_hour;
            std::stringstream(time.substr(OFFSET_MINUTE, 2)) >> timeLocal.tm_min;
            std::stringstream(time.substr(OFFSET_SECOND, 2)) >> timeLocal.tm_sec;

            timeLocal.tm_isdst = -1;

            return mktime(&timeLocal);
        }

        [[maybe_unused]] static std::string ndays_off(const std::string& t, std::int32_t n) {
            auto s = to_time_t(t);
            s = DatetimeUtil::ndays_off(s, n);
            return from_time_t(s);
        }

        [[maybe_unused]] static std::string from_time_t(std::time_t t) {
            std::tm tm{};
            localtime_r(&t, &tm);
            return from_tm(tm);
        }

        [[maybe_unused]] static std::string from_tm(const std::tm& tm) {
            char buff[32] = {0};
            auto n = snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1,
                              tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            if (n >= sizeof(buff)) {
                buff[sizeof(buff) - 1] = 0;
            }
            return buff;
        }

        [[maybe_unused]] static std::pair<std::string, std::string> endpoints() {
            auto seconds = std::time(nullptr);

            return endpoints(seconds);
        }

        [[maybe_unused]] static std::pair<std::string, std::string> endpoints(time_t t) {
            auto t1 = DatetimeUtil::trunc_to_day(t);
            auto t2 = DatetimeUtil::ndays_off(t1, 1) - 1;

            auto s1 = from_time_t(t1);
            auto s2 = from_time_t(t2);

            return {s1, s2};
        }

        // t2 - t1
        [[maybe_unused]] static int32_t diff_day(const std::string& t1, const std::string& t2) {
            auto s1 = to_time_t(t1);
            auto s2 = to_time_t(t2);
            return DatetimeUtil::diff_day(s1, s2);
        }

        // yyyy-mm-dd [hh24:mi:ss] vs yyyy-mm-dd [hh24:mi:ss]
        [[maybe_unused]] static bool is_same_date(const std::string& t1, const std::string& t2) {
            return t1.substr(0, 10) == t2.substr(0, 10);
        }

        // yyyymmdd[hh24miss] vs yyyymmdd[hh24miss]
        [[maybe_unused]] static bool is_same_minute(const std::string& t1, const std::string& t2) {
            return t1.substr(0, 16) == t2.substr(0, 16);
        }

        [[maybe_unused]] static std::string trunc_to_day(const std::string& t) {
            auto s = t.substr(0, 11);
            s.append("00:00:00");
            return s;
        }

        [[maybe_unused]] static std::string trunc_to_hour(const std::string& t) {
            auto s = t.substr(0, 14);
            s.append("00:00");
            return s;
        }

        [[maybe_unused]] static std::int32_t hour(const std::string& t) {
            auto s = t.substr(OFFSET_HOUR, 2);
            return (std::int32_t)strtol(s.c_str(), nullptr, 10);
        }

        [[maybe_unused]] static bool is_between(const std::string& t, const std::string& lower_bound,
                                                const std::string& upper_bound) {
            return t >= lower_bound && t < upper_bound;
        }
    };
}; // namespace app_common

#endif // PRESAMPLER_VIBRATION_DATETIME19UTIL_HPP
