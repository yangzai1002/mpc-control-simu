/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * History:
 * xy          2019/03/28    1.0.0        refer to apollo3.5
 *****************************************************************************/
#ifndef AVOS_TIME_DURATION_H_
#define AVOS_TIME_DURATION_H_

#include <math.h>
#include <stdint.h>
#include <climits>
#include <iostream>
#include <thread>
#include <iomanip>  
#include <stdexcept>

namespace avos {
namespace common {

class Duration {
public:
    Duration() {}
    explicit Duration(int64_t nanoseconds) { nanoseconds_ = nanoseconds; }
    explicit Duration(int nanoseconds) {
        nanoseconds_ = static_cast<int64_t>(nanoseconds);
    }
    explicit Duration(double seconds) {
        nanoseconds_ = static_cast<int64_t>(seconds * 1000000000UL);
    }
    explicit Duration(uint32_t seconds, uint32_t nanoseconds) {
        nanoseconds_ = static_cast<uint64_t>(seconds) * 1000000000UL + nanoseconds;
    }
    Duration(const Duration &other) { nanoseconds_ = other.nanoseconds_; }
    Duration &operator=(const Duration &other) {
        this->nanoseconds_ = other.nanoseconds_;
        return *this;
    }
    ~Duration() {}

    double ToSecond() const {
        return static_cast<double>(nanoseconds_) / 1000000000UL;
    }
    int64_t ToNanosecond() const { return nanoseconds_; }
    bool IsZero() const { return nanoseconds_ == 0; }
    void Sleep() const {
        auto sleep_time = std::chrono::nanoseconds(nanoseconds_);
        std::this_thread::sleep_for(sleep_time);
    }

    Duration operator+(const Duration &rhs) const {
        return Duration(nanoseconds_ + rhs.nanoseconds_);
    }
    Duration operator-(const Duration &rhs) const {
        return Duration(nanoseconds_ - rhs.nanoseconds_);
    }
    Duration operator-() const { return Duration(-nanoseconds_); }
    Duration operator*(double scale) const {
        return Duration(int64_t(static_cast<double>(nanoseconds_) * scale));
    }
    Duration &operator+=(const Duration &rhs) {
        *this = *this + rhs;
        return *this;
    }
    Duration &operator-=(const Duration &rhs) {
        *this = *this - rhs;
        return *this;
    }
    Duration &operator*=(double scale) {
        *this = Duration(int64_t(static_cast<double>(nanoseconds_) * scale));
        return *this;
    }
    bool operator==(const Duration &rhs)  const {
        return nanoseconds_ == rhs.nanoseconds_;
    }
    bool operator!=(const Duration &rhs) const {
        return nanoseconds_ != rhs.nanoseconds_;
    }
    bool operator>(const Duration &rhs) const {
        return nanoseconds_ > rhs.nanoseconds_;
    }
    bool operator<(const Duration &rhs) const {
        return nanoseconds_ < rhs.nanoseconds_;
    }
    bool operator>=(const Duration &rhs) const {
        return nanoseconds_ >= rhs.nanoseconds_;
    }
    bool operator<=(const Duration &rhs) const {
        return nanoseconds_ <= rhs.nanoseconds_;
    }

private:
    int64_t nanoseconds_ = 0;
};


}  // namespace common
}  // namespace avos

#endif  // AVOS_TIME_DURATION_H_
