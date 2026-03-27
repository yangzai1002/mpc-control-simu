#ifndef __YMDMS2GPSTS_HPP__
#define __YMDMS2GPSTS_HPP__
#include <chrono>
#include <ctime>
#include <iostream>
#include <vector>

// 闰秒列表（截至2023年7月）

// 计算指定UTC时间点的累计闰秒数
inline int getLeapSeconds(const std::chrono::system_clock::time_point& utcTime) {
    const std::vector<std::time_t> leapSeconds = {
        // 格式：UTC时间点（自1970-01-01以来的秒数），该时间点后增加1闰秒
        46828800,   // 1981-07-01
        78364800,   // 1982-07-01
        109900800,  // 1983-07-01
        173059200,  // 1985-07-01
        252028800,  // 1988-01-01
        315187200,  // 1990-01-01
        346723200,  // 1991-01-01
        393984000,  // 1992-07-01
        425520000,  // 1993-07-01
        457056000,  // 1994-07-01
        504489600,  // 1996-01-01
        551750400,  // 1997-07-01
        599184000,  // 1999-01-01
        820108800,  // 2006-01-01
        914803200,  // 2009-01-01
        1025136000, // 2012-07-01
        1119744000, // 2015-07-01
        1230768000  // 2017-01-01
    };
    std::time_t utcSeconds = std::chrono::system_clock::to_time_t(utcTime);
    int         count = 0;
    for (auto leap : leapSeconds) {
        if (utcSeconds >= leap) {
            count++;
        }
    }
    return count;
}

// GPS起始时间点（1980-01-06 00:00:00 UTC）
inline std::chrono::system_clock::time_point gpsEpoch() {
    std::tm tm = {};
    tm.tm_year = 80; // 1980年
    tm.tm_mon = 0;   // 1月
    tm.tm_mday = 6;  // 6日
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

// 结构体表示GPS周和周内秒
struct GPSTime {
    int    week; // GPS周数
    double tow;  // 周内秒（包含小数部分，精度到毫秒）

    GPSTime(int w, double s) : week(w), tow(s) {
    }
};

struct TimeWithMillis {
    std::tm tm;     // 年月日时分秒
    int     millis; // 毫秒 (0-999)
};

inline TimeWithMillis getCurrentTimeWithMillis(int diff_ms = 0) {
    // 获取当前时间点（带毫秒精度）
    auto now = std::chrono::system_clock::now();
    auto now_new = now;
    if (diff_ms > 0) // at t=0 gnss send data, at t=2 recv data, diff=2-1=1, diff>0
    {
        now_new = now - std::chrono::milliseconds(100); // ts when gnss send data
    } else if (diff_ms < 0) {
        now_new = now + std::chrono::milliseconds(100);
    }

    // 转换为time_t（秒级精度）
    std::time_t now_c = std::chrono::system_clock::to_time_t(now_new);

    // 计算毫秒部分
    auto millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now_new.time_since_epoch());
    int  millis = static_cast<int>(millis_since_epoch.count() % 1000);

    // 转换为本地时间（丢弃毫秒）
    TimeWithMillis result;
    // result.tm = *std::localtime(&now_c);  // 注意：std::localtime非线程安全
    result.tm = *std::gmtime(&now_c); // 注意：std::localtime非线程安全
    result.millis = millis;

    return result;
}

// 将UTC时间（带毫秒）转换为GPS周和周内秒
inline GPSTime utcToGps(int year, int month, int day, int hour, int minute, int second, int millisecond) {
    if (1900 >= year) {
        int    week = 0;
        double tow = 0;
        return {week, tow};
    }
    // 构建UTC时间点
    std::tm utc_tm = {};
    utc_tm.tm_year = year - 1900;
    utc_tm.tm_mon = month - 1;
    utc_tm.tm_mday = day;
    utc_tm.tm_hour = hour;
    utc_tm.tm_min = minute;
    utc_tm.tm_sec = second;

    // 转换为time_point
    auto utc_time = std::chrono::system_clock::from_time_t(std::mktime(&utc_tm));
    // 添加毫秒部分
    utc_time += std::chrono::milliseconds(millisecond);

    // 计算GPS起始时间点
    auto gps_epoch = gpsEpoch();

    // 计算UTC时间与GPS起始时间的差值（包含闰秒）
    auto   duration = std::chrono::duration_cast<std::chrono::milliseconds>(utc_time - gps_epoch);
    double total_seconds = duration.count() / 1000.0;

    // 计算该时刻的累计闰秒数
    int leap_seconds = getLeapSeconds(utc_time);

    // GPS时间 = UTC时间 + 闰秒
    double gps_seconds = total_seconds + leap_seconds;

    // 计算GPS周和周内秒
    const int SECONDS_PER_WEEK = 604800;
    int       week = static_cast<int>(gps_seconds / SECONDS_PER_WEEK);
    double    tow = gps_seconds - week * SECONDS_PER_WEEK;

    return {week, tow};
}

inline GPSTime utcToGps(TimeWithMillis cur_ts) {
    // 转换为time_point
    auto utc_time = std::chrono::system_clock::from_time_t(std::mktime(&(cur_ts.tm)));
    // 添加毫秒部分
    utc_time += std::chrono::milliseconds(cur_ts.millis);

    // 计算GPS起始时间点
    auto gps_epoch = gpsEpoch();

    // 计算UTC时间与GPS起始时间的差值（包含闰秒）
    auto   duration = std::chrono::duration_cast<std::chrono::milliseconds>(utc_time - gps_epoch);
    double total_seconds = duration.count() / 1000.0;

    // 计算该时刻的累计闰秒数
    int leap_seconds = getLeapSeconds(utc_time);

    // GPS时间 = UTC时间 + 闰秒
    double gps_seconds = total_seconds + leap_seconds;

    // 计算GPS周和周内秒
    const int SECONDS_PER_WEEK = 604800;
    int       week = static_cast<int>(gps_seconds / SECONDS_PER_WEEK);
    double    tow = gps_seconds - week * SECONDS_PER_WEEK;

    return {week, tow};
}

#endif
