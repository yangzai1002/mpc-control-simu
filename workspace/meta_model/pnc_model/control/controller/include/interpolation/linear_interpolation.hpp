#ifndef INTERPOLATION__LINEAR_INTERPOLATION_HPP_
#define INTERPOLATION__LINEAR_INTERPOLATION_HPP_

#include "interpolation/interpolation_utils.hpp"

#include <vector>

namespace interpolation
{


//
double lerp(const double src_val, const double dst_val, const double ratio);


//
std::vector<double> lerp(const std::vector<double> &base_keys,
        const std::vector<double> &base_values,
        const std::vector<double> &query_keys);


//
double lerp(const std::vector<double> &base_keys,
        const std::vector<double> &base_values, const double query_key);



}  // namespace

#endif  // INTERPOLATION__LINEAR_INTERPOLATION_HPP_
