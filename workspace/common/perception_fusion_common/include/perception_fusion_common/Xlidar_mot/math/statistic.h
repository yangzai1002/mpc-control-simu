/*
 * Copyright (c) 2015-2020 idriverplus(Beijing ZhiXingZhe Inc.)
 * website: www.idriverplus.com
 * author: wangxiao
 * update log:
 * 20170501:
 * 1) make up the basic static algorithm framework
*/

#pragma once
#include <vector>

namespace avos {
namespace perception {
namespace xlidarMOT { 
namespace math {

template <typename T>
class Statistic {
public:
  //description statistic
  static T getMax(const std::vector<T>& input) const;
  static T getMin(const std::vector<T>& input) const;
  static T getSum(const std::vector<T>& input) const;
  static double getMean(const std::vector<T>& input) const;
  static double getVariance(const std::vector<T>& input) const;
  static double getStdDev(const std::vector<T>& input) const;
};

}
}  // namespace math
}  // namespace perception
}  // namespace avos

#include "statistic.hpp"


