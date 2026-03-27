#ifndef __TIME_COST__
#define __TIME_COST__

#include <chrono>
#include <iostream>
#include <sstream>

#define TIME_COST_START(NAME) \
    auto __start_##NAME = std::chrono::high_resolution_clock::now();

#define TIME_COST_END(NAME) \
        auto __end_##NAME = std::chrono::high_resolution_clock::now(); \
        auto __dur_##NAME = std::chrono::duration_cast<std::chrono::microseconds>(__end_##NAME - __start_##NAME).count(); \
        std::ostringstream __oss_##NAME; \
        __oss_##NAME << "[TIME] " #NAME " cost: " << __dur_##NAME / 1000.0 << " ms"; \
        std::cout << __oss_##NAME.str() << std::endl; \

#define TIME_COST_END_WITH_ECHO(NAME, VAL) \
        auto __end_##NAME = std::chrono::high_resolution_clock::now(); \
        auto __dur_##NAME = std::chrono::duration_cast<std::chrono::microseconds>(__end_##NAME - __start_##NAME).count(); \
        std::ostringstream __oss_##NAME; \
        __oss_##NAME << "[TIME] " #NAME "--" << VAL << " cost: " << __dur_##NAME / 1000.0 << " ms"; \
        std::cout << __oss_##NAME.str() << std::endl; \

#define TIME_COST(NAME, CODE_BLOCK) \
        TIME_COST_START(NAME); \
        CODE_BLOCK; \
        TIME_COST_END(NAME); \

inline double current_time_s() {
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    return static_cast<double>(ns.count()) / 1e9f;
}

#endif