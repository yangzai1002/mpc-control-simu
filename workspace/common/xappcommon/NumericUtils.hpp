//
// Created by yixingwang on 2023/11/22.
//

#ifndef COMMON_UTILS_NUMERIC_UTILS_HPP
#define COMMON_UTILS_NUMERIC_UTILS_HPP

#include <fstream>
#include <string>

namespace app_common {
    class NumericUtils {
    public:
        NumericUtils() = delete;

        static std::uint32_t next_2complement_larger(std::uint32_t n) {
            // 如果 n 已经是 2^n，直接返回 n
            if ((n & (n - 1)) == 0) {
                return n;
            }

            // 找到比 n 大的最接近的 2^n
            unsigned int power = 1;
            while (power <= n) {
                power <<= 1;
            }

            return power;
        }
    };
} // namespace app_common

#endif