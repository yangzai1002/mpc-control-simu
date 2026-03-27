#pragma once
#include <algorithm>
#include "log/log.h"
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <list>
#include <mutex>
#include <vector>
#include <assert.h>
#include "common/time/node_time.h"
#include "fusion_type.h"
#include "perception_fusion_common/common/lidarvisioncommon.h"
namespace avos {
namespace perception {
namespace xlidarMOT{
// using TrafficImage = sensor_msgs::Image; //add by lxd

#define ERROR_LIDAR_INIT_ERROR (-100)
#define ERROR_RADAR_INIT_ERROR (-200)
#define ERROR_VISION_INIT_ERROR (-300)

#define ERROR_FUSION_INIT_ERROR (-400) //level 2: program error!
#define ERROR_FUSION_NO_RESULT  (-401) //level 0: fusion have no result yet.


// #define LIDAR_SOURCE 0x0100
// #define RADAR_SOURCE 0x0200
// #define VISION_SOURCE 0x0400
#define VISION_LIDAR_SOURCE 0x0500

#define SOURCE_MASK 0xFF00

#define ULTRA_SOURCE 0x0300

#define FUSION_SENSOR_DATA_SIZE 5
#define VISION_ID 0
#define LIDAR_ID 1
#define RADAR_ID 2
#define DOR_ID 3 // for lidar+vision static-dynamic obj detection

#define SENSOR_NUMS 9

//融合中的目标类型，修改为和视觉类型一致 gqp 20240925
#define CAR_TYPE 2 // 0
#define TRUCK_TYPE 4 // 1
#define PEDESTRIAN_TYPE 0 // 2
#define CYCLIST_TYPE 1 // 3
#define UNKNOWN_TYPE 13 // 4
// #define UNKNOWN_MOVABLE_TYPE 5
#define UNKNOWN_UNMOVABLE_TYPE 26 //6
#define SUBULATE_TYPE 10 // 7 //lxd：新增类别锥形桶
#define ROADBLOCK_TYPE 12 // 8//lxd:新增类别路障(水马)
#define ANTITANK_TYPE 27 //暂未实现，先改成一个可用的27, lxd使用的旧版为9//lxd:反坦克锥
#define WATERFENCE_TYPE 28//lxd:反坦克锥
#define UNDEFINED_TYPE 255

#define HOUR_SECONDS 3600

#define FOLLOW_TASK_DIR_ID (-88)

// thread id
#define LIDAR_TOPCENTER_THREAD_ID 0
#define LIDAR_LEFT_THREAD_ID 1
#define LIDAR_RIGHT_THREAD_ID 2
#define LIDAR_FRONT_THREAD_ID 3
#define LIDAR_REAR_THREAD_ID 4

#define LIDAR_PROCESS_THREAD_ID 5
#define RADAR_PROCESS_THREAD_ID 6
#define DOR_PROCESS_THREAD_ID 7
#define LIDAR2CAM_PROCESS_THREAD_ID 8
#define FUSION_THREAD_ID 9
#define DRIVE_FUSION_THREAD_ID 10
#define ULTRA_PROCESS_THREAD_ID 11
#define UPDATE_CONFIG_THREAD_ID 12

//视觉深度检测-目标类型:
#define VISION_PEOPLE 0         // 人
#define VISION_RIDER 1          // 骑行者
#define VISION_CAR 2            // 乘用车
#define VISION_BUS 3            // 公交车
#define VISION_TRUCK 4          // 货车
#define VISION_CROWD 5          // 人群
#define VISION_TRICYCLE 6       // 三轮车
#define VISION_BIKE 7           // 两轮车
#define VISION_TRICYCLE_RIDER 8 // 人骑机动三轮车
#define VISION_BIKE_RIDER 9     // 人骑摩托车
#define VISION_SUBULATE 10      // 锥形筒
#define VISION_TRIANGLE 11      // 三角牌
#define VISION_ROADBLOCK 12     // 路障
#define VISION_OTHER 13

#define VISION_WHEEL 14       // 车轮
#define VISION_WHEEL_POINT 15 // 车轮接地点
#define VISION_PERMISSION_ARROW     16 // 允许通行<左右转、直行、等>，对下游输出，合并后类型为arrow
#define VISION_SPEED_LIMIT 17       // 限速，合并到19
#define VISION_FORBISSANCE_ARROW 18 // 禁止通行<左右转、直行、等>，合并到16
#define VISION_WORD 19              // 普通文字，对下游输出
#define VISION_NOTICE 20 // 人行横道、减速慢行<倒三角>，合并到16

#define VISION_ISOLATION_PILE 21 // 隔离桩
#define VISION_SHIDUN 22         // 石墩
#define VEHICLE_HEAD 23 // 车头
#define VEHICLE_TAIL 24 // 车尾
#define VISION_TRAFFICSIGN 25 // 交通标识牌
#define VISION_UNSUPPORT 255 // 不支持，不输出

// Defines an annotated mutex that can only be locked through its scoped locker
// implementation.
class Mutex {
public:
  // A RAII class that acquires a mutex in its constructor, and
  // releases it in its destructor. It also implements waiting functionality on
  // conditions that get checked whenever the mutex is released.
  class Locker {
  public:
    Locker(Mutex *mutex) : mutex_(mutex), lock_(mutex->mutex_) {}

    ~Locker() {
      lock_.unlock();
      mutex_->condition_.notify_all();
    }

    void Await() { mutex_->condition_.wait(lock_); }

    bool AwaitWithTimeout(std::chrono::milliseconds timeout) {
      return (mutex_->condition_.wait_for(lock_, timeout) ==
              std::cv_status::no_timeout);
    }

  private:
    Mutex *mutex_;
    std::unique_lock<std::mutex> lock_;
  };

private:
  std::condition_variable condition_;
  std::mutex mutex_;
};

template <class T>
struct TimeData {
  bool is_consumed;
  double rev_time;
  T data;
  TimeData() {
    is_consumed = false;
    rev_time = 0.0;
  }
};

// template <class T>
// struct DataCache {
//   std::list<TimeData<T>> data_cache;
//   int max_length;
//   std::mutex mutex_;
//   DataCache() {
//     data_cache.clear();
//     max_length = 10;
//   }
//   void Clear() { data_cache.clear(); }

//   void setConfig(int length = 10) {
//     if (length <= 0) length = 10;
//     data_cache.clear();
//     max_length = length ;
//   }
//   DataCache(int length) {
//     if (length <= 0) length = 10;
//     data_cache.clear();
//     max_length = length ;
//   }

//   void Push(const T &data) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     TimeData<T> temp;
//     temp.rev_time = avos::common::NodeTime::Now().ToSecond();
//     // temp.rev_time = 0;
//     temp.data = data;
//     data_cache.push_back(std::move(temp));
//     while (data_cache.size() > max_length) {
//       data_cache.erase(data_cache.begin());
//     }
//   }

//   int PopNewest(T &data) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     data = data_cache.back().data;
//     // data_cache.pop_back();
//     data_cache.clear();
//     return 0;
//   }

//   int GetNewest(T &data) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     data = data_cache.back().data;
//     return 0;
//   }

//   int GetSpecified(T &data, int frames_to_current) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     if (data_cache.size() < frames_to_current) {
//       data = data_cache.back().data;
//       return 0;
//     } else {
//       int idx = frames_to_current + 1;
//       data = std::prev(data_cache.end(), idx)->data;
//       return 0;
//     }
//   }

//   int GetNearest(T &data, double time, double threshsec=-1) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     for (int s = 1; s < data_cache.size(); s++) {
//       double t_new = std::prev(data_cache.end(), s)->data.time;
//       double t_old = std::prev(data_cache.end(), (s + 1))->data.time;
//       if (t_new >= time && t_old <= time) {
//         if ((time - t_old) > (t_new - time)) {
//           if(threshsec <= 0 || (threshsec > 0 && fabs(std::prev(data_cache.end(), s)->data.time - time) < threshsec))
//             data = std::prev(data_cache.end(), s)->data;
//         }
//         else {
//           if(threshsec <= 0 || (threshsec > 0 && fabs(std::prev(data_cache.end(), s+1)->data.time - time) < threshsec))
//             data = std::prev(data_cache.end(), (s+1))->data;
//         }
//         return 0;
//       }
//     }
//     if(threshsec <= 0 || (threshsec > 0 && fabs(data_cache.back().data.time - time) < threshsec))
//       data = data_cache.back().data;
//     if(threshsec > 0 && fabs(data.time - time) > threshsec)
//       return -1;
//     return 0;
//   }

//   int GetWithinThreshold(T &data, double target_time, float threshold_ms) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     for (int s = 1; s < data_cache.size(); s++) {
//       double time = std::prev(data_cache.end(), s)->data.time;
//       float dt_ms = (target_time - time) * 1000;
//       // 米文时间同步： 激光-米文时间=50ms
//       if (dt_ms <= threshold_ms && dt_ms > 0) {
//         data = std::prev(data_cache.end(), s)->data;
//         return 0;
//       }
//     }
//     return -1;
//   }

//   int GetWithinThreshold(T &data, double target_time, float upper_threshold_ms, float lower_threshold_ms) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     for (int s = 1; s < data_cache.size(); s++) {
//       double time = std::prev(data_cache.end(), s)->data.time_triggered;
//       float dt_ms = (target_time - time) * 1000;
//       // 米文时间同步： 激光-米文时间～50ms
//       // Brainbox时间同步： fabs（激光-米文）时间～0ms
//       if (dt_ms <= upper_threshold_ms && dt_ms >= lower_threshold_ms) {
//         data = std::prev(data_cache.end(), s)->data;
//         return 0;
//       }
//     }
//     return -1;
//   }

//   int GetOldest(T &data) {
//     std::unique_lock<std::mutex> locker(mutex_);
//     if (data_cache.size() <= 0) return -1;
//     data = data_cache.front().data;
//     return 0;
//   }

//   void PrintDetail(const std::string &prefix) {
//     AINFO  << prefix << " data_cache:" << data_cache.size();
//   }

//   int GetDataSize() {
//     return data_cache.size();
//   }

//   bool CheckValid() {
//     if (data_cache.size() <= 0) return false;
//     return true;
//   }
// };

template <class T>
class DataCache {
public:
  DataCache() {
    data_cache.clear();
    max_length = 11;
    valid_times = 5;
    push_time_gap_ms = 500;
    rm_time_gap_ms = 10000;
  }
  void Clear() { std::unique_lock<std::mutex> locker(mutex_);data_cache.clear(); }

  void setConfig(double _rm_time_gap_ms = 10000, double _push_time_gap_ms = 500,
                 int length = 10, int _valid_times = 5) {
    if (length <= 0) length = 10;
    data_cache.clear();
    max_length = length + _valid_times;
    valid_times = _valid_times;
    push_time_gap_ms = _push_time_gap_ms;
    rm_time_gap_ms = _rm_time_gap_ms;
  }
  void setConfig(int length = 10) {
    if (length <= 0) length = 10;
    data_cache.clear();
    max_length = length;
    valid_times = 0;
  }
  DataCache(double _rm_time_gap_ms, double _push_time_gap_ms, int length,
            int _valid_times) {
    if (length <= 0) length = 10;
    data_cache.clear();
    max_length = length + _valid_times;
    valid_times = _valid_times;
    push_time_gap_ms = _push_time_gap_ms;
    rm_time_gap_ms = _rm_time_gap_ms;
  }

  void Push(const T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    TimeData<T> temp;
    temp.rev_time = avos::common::NodeTime::Now().ToSecond();
    temp.data = data;
    data_cache.push_back(std::move(temp));
    while (data_cache.size() > max_length) {
      data_cache.erase(data_cache.begin());
    }

    condition_.notify_all();
  }

  bool ReadWithTimeout(T &out_data, const std::chrono::milliseconds timeout)
  {
    std::unique_lock<std::mutex> locker(mutex_);
    if(condition_.wait_for(locker, timeout) !=
              std::cv_status::no_timeout) 
    {
      return false;
    }
    if (data_cache.size() <= 0) return false;
    out_data = data_cache.back().data;
    return true;
  }

  int GetNewest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    data = data_cache.back().data;
    return 0;
  }
  int GetSecNewest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    if(data_cache.size() == 1)
      data = data_cache.back().data;
    else 
      data = std::prev(data_cache.end(), 2)->data;
    return 0;
  }

  int GetSpecified(T &data, int frames_to_current) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    if (data_cache.size() < frames_to_current) {
      data = data_cache.back().data;
      return 0;
    } else {
      int idx = frames_to_current + 1;
      data = std::prev(data_cache.end(), idx)->data;
      return 0;
    }
  }

  double GetNewestTime() {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    return data_cache.back().data.time;
  }
  double GetNewestHeaderTime() {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    return data_cache.back().data.header.time;
  }

  double GetPrevTime(double querytime) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    double ret = data_cache.front().data.time; // 第一个的时间戳
    for (int s = 1; s < data_cache.size(); s++) {
      double t_new = std::prev(data_cache.end(), s)->data.time;
      double t_old = std::prev(data_cache.end(), (s + 1))->data.time;
      if (t_new >= querytime && t_old < querytime)
        return t_old;
    }
    return ret;
  }

  double GetNextTime(double querytime) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    double ret = data_cache.back().data.time; // 最后一个的时间戳
    for (int s = 1; s < data_cache.size(); s++) {
      double t_new = std::prev(data_cache.end(), s)->data.time;
      double t_old = std::prev(data_cache.end(), (s + 1))->data.time;
      if (t_new > querytime && t_old <= querytime)
        return t_new;
    }
    return ret;
  }

  int GetNearest(T &data, double time, double threshsec=-1) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    for (auto it=data_cache.rbegin(); it!= data_cache.rend(); ++it) {
      double t_new = it->data.time;
      auto pre=it;
      if(++pre==data_cache.rend()){
        break;
      }
      double t_old = pre->data.time;
      if (t_new >= time && t_old <= time) {
        if ((time - t_old) > (t_new - time)) {
          if(threshsec <= 0 || (threshsec > 0 && fabs(it->data.time - time) < threshsec))
          data = it->data;
        }
        else {
          if(threshsec <= 0 || (threshsec > 0 && fabs(pre->data.time - time) < threshsec))
          data = pre->data;
        }
        return 0;
      }
    }
    if(threshsec <= 0 || (threshsec > 0 && fabs(data_cache.back().data.time - time) < threshsec))
      data = data_cache.back().data;
    if(threshsec > 0 && fabs(data.time - time) > threshsec)
      return -1;
    return 0;

  }
int GetNearestbyHeaderTime(T &data, double time, double threshsec=-1) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    for (auto it=data_cache.rbegin(); it!= data_cache.rend(); ++it) {
      double t_new = it->data.header.time;
      auto pre=it;
      if(++pre==data_cache.rend()){
        break;
      }
      double t_old = pre->data.header.time;
      if (t_new >= time && t_old <= time) {
        if ((time - t_old) > (t_new - time)) {
          if(threshsec <= 0 || (threshsec > 0 && fabs(it->data.header.time - time) < threshsec))
          data = it->data;
        }
        else {
          if(threshsec <= 0 || (threshsec > 0 && fabs(pre->data.header.time - time) < threshsec))
          data = pre->data;
        }
        return 0;
      }
    }
    if(threshsec <= 0 || (threshsec > 0 && fabs(data_cache.back().data.header.time - time) < threshsec))
      data = data_cache.back().data;
    if(threshsec > 0 && fabs(data.header.time - time) > threshsec)
      return -1;
    return 0;
  }
  // 时间戳大于输入时间的最近的
  int GetNearestAfter(T &data, double time, double threshsec=-1) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    for (int s = 1; s < data_cache.size(); s++) {
      double t_new = std::prev(data_cache.end(), s)->data.time;
      double t_old = std::prev(data_cache.end(), (s + 1))->data.time;
      if (t_new - time > 1e-4 && t_old <= time) {
        if(threshsec <= 0 || (threshsec > 0 && fabs(std::prev(data_cache.end(), s)->data.time - time) < threshsec)) {
          data = std::prev(data_cache.end(), s)->data;
          return 0;
        }
      }
    }
    if(data_cache.back().data.time - time > 1e-4) {
      if(threshsec <= 0) {
        data = data_cache.back().data;
        return 0;
      }
      else {
        if(data_cache.back().data.time - time < threshsec) {
          data = data_cache.back().data;
          return 0;
        }
        else {
          return -1;
        }
      }
    }
    else {
      return -1;
    }
  }

  //找到对应时间后往前取front_num个，往后取back_num个 [front_num,back_num)
  //front_num=20,back_num=20 举例：输出->40个数  [1,2,3,...19,s,21,...,40]
  int GetNearestSquence(std::vector<T> &data, int front_num,int back_num,double time) {
    std::unique_lock<std::mutex> locker(mutex_);
    data.clear();
    if (data_cache.empty()) return -1;
    int idx = 0;
    int length = data_cache.size();
    for (int s = 1; s < data_cache.size(); s++) {
      double t_new = std::prev(data_cache.end(), s)->data.time;
      double t_old = std::prev(data_cache.end(), (s + 1))->data.time;
      if (t_new >= time && t_old <= time) {
        if ((time - t_old) > (t_new - time)) {
          idx = s;
        } else {
          idx = s+1;
        }
        //前5 后10
        int start_idx =  (s+front_num)>length?length:s+front_num;
        int end_idx = (s-back_num-1)<1?1:s-back_num-1;
        for(int i=end_idx;i<=start_idx;i++) {
          data.insert(data.begin(),std::prev(data_cache.end(), i)->data);
          // data.push_back(std::prev(data_cache.end(), i)->data);
        }
        return 0;
      }
    }
    int start_idx =  (1+front_num)>length?length:1+front_num;
    for(int i=1;i<=start_idx;i++)
      data.insert(data.begin(),std::prev(data_cache.end(), i)->data);
      // data.push_back(std::prev(data_cache.end(), i)->data);
    return 0;
  }

  int GetAllData(std::vector<T> &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    data.clear();
    if (data_cache.empty()) return -1;
    for (size_t i = 0; i < data_cache.size(); i++) {
      data.insert(data.begin(),std::next(data_cache.begin(),i)->data);
    }
    data_cache.clear();
    return 0;
  }

  int GetWithinThreshold(T &data, double target_time, float upper_threshold_ms, float lower_threshold_ms) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    for (int s = 1; s < data_cache.size(); s++) {
      double time = std::prev(data_cache.end(), s)->data.time;
      float dt_ms = (target_time - time) * 1000;
      // 米文时间同步： 激光-米文时间～50ms
      // Brainbox时间同步： fabs（激光-米文）时间～0ms
      if (dt_ms <= upper_threshold_ms && dt_ms >= lower_threshold_ms) {
        data = std::prev(data_cache.end(), s)->data;
        return 0;
      }
    }
    return -1;
  }
  int GetWithinThresholdHeaderTime(T &data, double target_time, float upper_threshold_ms, float lower_threshold_ms) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    for (int s = 1; s < data_cache.size(); s++) {
      double time = std::prev(data_cache.end(), s)->data.header.time;
      float dt_ms = (target_time - time) * 1000;
      // 米文时间同步： 激光-米文时间～50ms
      // Brainbox时间同步： fabs（激光-米文）时间～0ms
      if (dt_ms <= upper_threshold_ms && dt_ms >= lower_threshold_ms) {
        data = std::prev(data_cache.end(), s)->data;
        return 0;
      }
    }
    return -1;
  }
  int GetOldest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= valid_times) return -1;
    data = data_cache.front().data;
    return 0;
  }

  void PrintDetail(const std::string &prefix) {
    std::cout << prefix << " data_cache:" << data_cache.size() << std::endl;
  }

  int GetDataSize() {
    return data_cache.size();
  }

  bool CheckValid() {
    if (data_cache.size() <= valid_times) return false;
    return true;
  }
  int GetMaxLength()
  {
    return max_length;
  }
private:
  std::list<TimeData<T>> data_cache;
  int max_length;
  int valid_times;
  double push_time_gap_ms;
  double rm_time_gap_ms;
  std::mutex mutex_;
  std::condition_variable condition_;
};

template <class T>
struct VecCache {
  std::vector<TimeData<T>> data_cache;
  int max_length;
  std::mutex mutex_;
  VecCache() {
    data_cache.clear();
    max_length = 11;
  }
  int Size() {
    return data_cache.size();
  }
  VecCache& operator=(const VecCache& obj)
  {
    data_cache = obj.data_cache;
    max_length = obj.max_length;
    return *this;
  }
  VecCache(const VecCache &obj)
  {
    data_cache = obj.data_cache;
    max_length = obj.max_length;
  }


  void setConfig(int length = 10) {
    if (length <= 0) length = 10;
    data_cache.clear();
    max_length = length ;
    data_cache.reserve(max_length + 1);
  }
  VecCache(int length) {
    if (length <= 0) length = 10;
    data_cache.clear();
    max_length = length ;
    data_cache.reserve(max_length + 1);
  }

  void Push(const T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    TimeData<T> temp;
    temp.rev_time = avos::common::NodeTime::Now().ToSecond();
    // temp.rev_time = 0 ;
    temp.data = data;
    data_cache.push_back(std::move(temp));
    if (data_cache.size() > max_length) {
      data_cache.erase(data_cache.begin());
    }
  }

  int PopNewest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    data = data_cache.back().data;
    if (data_cache.size() > 0) {
      data_cache.pop_back();
    }

    return 0;
  }

  int GetNewest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    data = data_cache.back().data;
    return 0;
  }
  int PopOldest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    data = data_cache.front().data;
    data_cache.erase(data_cache.begin());
    return 0;
  }
  int GetOldest(T &data) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (data_cache.size() <= 0) return -1;
    data = data_cache.front().data;
    return 0;
  }
  void PrintDetail(const std::string &prefix) {
    AINFO  << prefix << " cache size:" << data_cache.size();
    AINFO ;
  }
  inline size_t size() { return data_cache.size(); }
  T &operator[](const size_t i) {
    assert(i >= 0 && i < data_cache.size());
    return data_cache[i].data;
  }

  void Clear() { data_cache.clear(); }
  bool CheckValid() {
    if (data_cache.size() <= 0) return false;
    return true;
  }
};
 struct PerceptionLocType {
  bool valid=false;
  double time;
  double lon = 0.0;
  double lat = 0.0;
  double xg = 0.0;
  double yg = 0.0;
  double zg = 0.0;
  double yawrate = 0.0;
  float angle = 0.0;
  float pitch = 0.0;
  float roll = 0.0;
  float vx = 0.0;
  float vy = 0.0;
  float accx = 0.0;
  float accy = 0.0;
  float speed = 0.0;
  Eigen::Vector3d ego_t_g = Eigen::Vector3d(0,0,0);
  Eigen::Matrix3d ego_R_g = Eigen::Matrix3d::Identity();
  Eigen::Matrix3d ego_R_g_Inverse = Eigen::Matrix3d::Identity();

  double dr_time;
  double dr_x = 0.0;
  double dr_y = 0.0;
  double dr_z = 0.0;
  float dr_roll = 0.0;
  float dr_pitch = 0.0;
  float dr_yaw = 0.0;
  Eigen::Vector3d ego_t = Eigen::Vector3d(0,0,0);
  Eigen::Matrix3d ego_R = Eigen::Matrix3d::Identity();
  Eigen::Matrix3d ego_R_Inverse = Eigen::Matrix3d::Identity();
};

 struct ObjDetectBoxes {
  std::string camera_name;
  unsigned int x, y, w, h;  // (x,y) - top-left corner, (w, h) - width & height of bounded box
  float prob;  // confidence - probability that the object was found correctly
  unsigned int obj_id;  // class of object - from range [0, classes-1]
  unsigned int orientation_id;
  bool track_enable;
  unsigned int  track_id;
  unsigned int  track_age;
  unsigned int  track_time_since_update;
  float track_score;
  std::vector<float> feature;
} ;

 struct ImageWithTime {
  double time_triggered;       // timestamp  seconds
  double time_received;       // timestamp  seconds
  cv::Mat image;
  void reset() {
    time_triggered = 0;
    time_received = 0;
    image.release();
  }
} ;

struct IdPoolSimple{
  int id=1000;
  void Set(const int& nid){
    if(nid<=0){
      id= -nid;
    }else{
      id=nid;
    }
  }
  int Get(){
    id = id+1;
    if(id<0){
      id=1;
    }
    return id;
  }
};

struct ObjDetectBoxesTime {
  std::string camera_name;
  double time_triggered;       // timestamp  seconds
  double time_received;       // timestamp  seconds
  bool image_valid[4];  // 0:front60  1: left90/pano_left  2:right90/pano_right  3:front30/pano_back
  // std::array<cv::Mat, 4> current_image;
  std::array<std::vector<ObjDetectBoxes>, 4> boxes_data;
  void reset() {
    time_triggered = 0;
    time_received = 0;
    image_valid[0] = false;
    image_valid[1] = false;
    image_valid[2] = false;
    image_valid[3] = false;
    boxes_data[0].clear();
    boxes_data[1].clear();
    boxes_data[2].clear();
    boxes_data[3].clear();
  }
};




}
}
}

