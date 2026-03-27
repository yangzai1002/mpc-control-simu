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
#ifndef AVOS_TIME_TIME_H_
#define AVOS_TIME_TIME_H_

#include <limits>
#include <string>
#include <sstream>
#include "duration.h"

namespace avos {
namespace common {

class NodeTime {
public:
  NodeTime() {}
  explicit NodeTime(uint64_t nanoseconds) {
    nanoseconds_ = nanoseconds;
    sec = static_cast<int32_t>(nanoseconds_ / 1000000000UL);
    nsec = nanoseconds_ - sec * 1000000000UL;
  }


  explicit NodeTime(int nanoseconds) {
    nanoseconds_ = static_cast<uint64_t>(nanoseconds);
    sec = static_cast<int32_t>(nanoseconds_ / 1000000000UL);
    nsec = nanoseconds_ - sec * 1000000000UL;
  }


  explicit NodeTime(double seconds) {
    nanoseconds_ = static_cast<uint64_t>(seconds * 1000000000UL);
    sec = static_cast<int32_t>(seconds);
    nsec = (seconds * 1000000000UL) - sec * 1000000000UL;
  }


  NodeTime(uint32_t seconds, uint32_t nanoseconds) {
    nanoseconds_ = static_cast<uint64_t>(seconds) * 1000000000UL + nanoseconds;
    sec = seconds;
    nsec = nanoseconds;
  }


  NodeTime(const NodeTime& other) { nanoseconds_ = other.nanoseconds_; }
  NodeTime& operator=(const NodeTime& other) {
    this->nanoseconds_ = other.nanoseconds_;
    return *this;
  }
  ~NodeTime() {}

  static NodeTime Now() {
    // auto now = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::system_clock::now();
    auto nano_time_point =
      std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    auto epoch = nano_time_point.time_since_epoch();
    uint64_t now_nano =
      std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
    //std::cout << "now_nano = " << now_nano << std::endl;
    return NodeTime(now_nano);
  }

  static NodeTime MonoTime() {
    auto now = std::chrono::steady_clock::now();
    auto nano_time_point =
      std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    auto epoch = nano_time_point.time_since_epoch();
    uint64_t now_nano =
      std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
    return NodeTime(now_nano);
  }

  static void SleepUntil(const NodeTime& time) {
      using clock = std::chrono::system_clock;
      auto ns = std::chrono::nanoseconds(time.ToNanosecond());

      // 从 epoch 开始构造 time_point
      clock::time_point tp{std::chrono::duration_cast<clock::duration>(ns)};
      std::this_thread::sleep_until(tp);
  }

  static uint64_t getSysBootTimeMs() {
    struct timespec uptime;
    clock_gettime(CLOCK_MONOTONIC, &uptime);
    uint64_t uptimeMs = uptime.tv_sec * 1000 + uptime.tv_nsec / 1000000;
    return uptimeMs;
  }
  double ToSecond() const {
    return static_cast<double>(nanoseconds_) / 1000000000UL;
  }
  int GetSec() const {
    return sec;
  }
  uint GetNsec() const {
    return nsec;
  }
  uint64_t ToNanosecond() const { return nanoseconds_; }
  std::string ToString() const {
      // 1. 把 nanoseconds_ 看成“自 1970-01-01 00:00:00 UTC 的时长”
      auto ns = std::chrono::nanoseconds(nanoseconds_);

      // 2. 转成 system_clock 的 duration 并构造 time_point
      using Clock = std::chrono::system_clock;
      Clock::time_point tp(
          std::chrono::duration_cast<Clock::duration>(ns));

      // 3. 取日历时间
      std::time_t time = Clock::to_time_t(tp);
      struct tm stm;
      struct tm* ret = localtime_r(&time, &stm);
      if (ret == nullptr) {          // 容错：转换失败就退化成秒
          return std::to_string(static_cast<double>(nanoseconds_) / 1'000'000'000.0);
      }

      // 4. 格式化
      std::stringstream ss;
  #if __GNUC__ >= 5
      ss << std::put_time(ret, "%F %T");
  #else
      char date_time[128];
      strftime(date_time, sizeof(date_time), "%F %T", ret);
      ss << date_time;
  #endif
      // 5. 追加纳秒部分
      ss << '.' << std::setw(9) << std::setfill('0') << (nanoseconds_ % 1'000'000'000UL);
      return ss.str();
  }
  bool IsZero() const { return nanoseconds_ == 0; }

  Duration operator-(const NodeTime& rhs) const {
    return Duration(static_cast<int64_t>(nanoseconds_ - rhs.nanoseconds_));
  }
  NodeTime operator+(const Duration& rhs) const {
    return NodeTime(nanoseconds_ + rhs.ToNanosecond());
  }
  NodeTime operator-(const Duration& rhs) const {
    return NodeTime(nanoseconds_ - rhs.ToNanosecond());
  }
  NodeTime& operator+=(const Duration& rhs) {
    *this = *this + rhs;
    return *this;
  }
  NodeTime& operator-=(const Duration& rhs) {
    *this = *this - rhs;
    return *this;
  }
  bool operator==(const NodeTime& rhs) const {
    return nanoseconds_ == rhs.nanoseconds_;
  }
  bool operator!=(const NodeTime& rhs) const {
    return nanoseconds_ != rhs.nanoseconds_;
  }

  bool operator>(const NodeTime& rhs) const {
    return nanoseconds_ > rhs.nanoseconds_;
  }
  bool operator<(const NodeTime& rhs) const {
    return nanoseconds_ < rhs.nanoseconds_;
  }

  bool operator>=(const NodeTime& rhs) const {
    return nanoseconds_ >= rhs.nanoseconds_;
  }

  bool operator<=(const NodeTime& rhs) const {
    return nanoseconds_ <= rhs.nanoseconds_;
  }

private:
  int sec = 0;
  uint nsec = 0;
  uint64_t nanoseconds_ = 0;
};


}  // namespace common
}  // namespace avos

#endif  // AVOS_TIME_TIME_H_
