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
#ifndef AVOS_TIME_RATE_H_
#define AVOS_TIME_RATE_H_

#include "duration.h"
#include "node_time.h"

namespace avos {
namespace common {

class Rate {
public:
  explicit Rate(double frequency): start_(NodeTime::Now()),
    expected_cycle_time_(1.0 / frequency),
    actual_cycle_time_(0.0) {}
  explicit Rate(uint64_t nanoseconds): start_(NodeTime::Now()),
    expected_cycle_time_(static_cast<int64_t>(nanoseconds)),
    actual_cycle_time_(0.0) {}
  explicit Rate(const Duration&): start_(NodeTime::Now()), expected_cycle_time_(d), actual_cycle_time_(0.0) {}
  void Sleep() {
    NodeTime expected_end = start_ + expected_cycle_time_;

    NodeTime actual_end = NodeTime::Now();

    // detect backward jumps in time
    if (actual_end < start_) {
      // AWARN << "Detect backward jumps in time";
      expected_end = actual_end + expected_cycle_time_;
    }

    // calculate the time we'll sleep for
    Duration sleep_time = expected_end - actual_end;

    // set the actual amount of time the loop took in case the user wants to kNow
    actual_cycle_time_ = actual_end - start_;

    // make sure to reset our start time
    start_ = expected_end;

    // if we've taken too much time we won't sleep
    if (sleep_time < Duration(0.0)) {
      // AWARN << "Detect forward jumps in time";
      // if we've jumped forward in time, or the loop has taken more than a full
      // extra
      // cycle, reset our cycle
      if (actual_end > expected_end + expected_cycle_time_) {
        start_ = actual_end;
      }
      // return false to show that the desired rate was not met
      return;
    }

    NodeTime::SleepUntil(expected_end);
  }
  void Reset() { start_ = NodeTime::Now(); }
  Duration CycleTime() const { return actual_cycle_time_; }
  Duration ExpectedCycleTime() const { return expected_cycle_time_; }

private:
  NodeTime start_;
  Duration expected_cycle_time_;
  Duration actual_cycle_time_;
};

}  // namespace common
}  // namespace avos

#endif  // AVOS_TIME_RATE_H_
