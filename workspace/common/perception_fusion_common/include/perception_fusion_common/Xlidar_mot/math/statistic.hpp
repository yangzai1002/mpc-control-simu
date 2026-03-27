/*
 * Copyright (c) 2015-2020 idriverplus(Beijing ZhiXingZhe Inc.)
 * website: www.idriverplus.com
 * author: wangxiao
 * update log:
 * 20170501:
 * 1) make up the basic static algorithm framework
*/
#pragma once
#include <algorithm>
#include <limits>
#include <cmath>


namespace avos {
namespace perception {
namespace xlidarMOT { 
namespace math {

template <typename T>
T Statistic::getMax(const std::vector<T>& input) const {
  if (input.empty()) {
    return std::numeric_limits<T>::lowest();
  }
  return *(std::max_element(input.begin(), input.end()));
}

template <typename T>
T Statistic::getMin(const std::vector<T>& input) const {
  if (input.empty()) {
    return std::numeric_limits<T>::max();
  }
  return *(std::min_element(input.begin(), input.end()));
}

template <typename T>
T Statistic::getSum(const std::vector<T>& input) const {
  return std::accumulate(input.begin(), input.end(), static_cast<T>(0));;
}

template <typename T>
double Statistic::getMean(const std::vector<T>& input) const {
  T sum = getSum(input);
  return sum * 1.0 / input.size();
}

template <typename T>
double Statistic::getVariance(const std::vector<T>& input) const {
  double mean = getMean(input);
  double temp = 0.0;
  for (size_t i = 0; i < input.size(); i++) {
    temp += (input[i] - mean) * (input[i] - mean);
  }
  return temp / input.size();
}

template <typename T>
double Statistic::getStdDev(const std::vector<T>& input) const {
  return sqrt(getVariance(input));
}

}
}  // namespace math
}  // namespace perception
}  // namespace avos
