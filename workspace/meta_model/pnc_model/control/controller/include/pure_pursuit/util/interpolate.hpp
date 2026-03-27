#ifndef LONGITUDINAL_CONTROLLER_BASE_HPP
#define LONGITUDINAL_CONTROLLER_BASE_HPP

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

namespace pure_pursuit
{

//
class LinearInterpolate
{

public:
    LinearInterpolate()
    {
    }

    ~LinearInterpolate()
    {
    }

    static bool interpolate(const std::vector<double> &base_index,
                            const std::vector<double> &base_value,
                            const std::vector<double> &return_index,
                            std::vector<double> &return_value);
};

//
class SplineInterpolate
{
    bool initialized_ = false;
    std::vector<double> a_;    //temporal vector for calculation
    std::vector<double> b_;
    std::vector<double> c_;
    std::vector<double> d_;

public:
    SplineInterpolate();

    explicit SplineInterpolate(const std::vector<double> &x);

    ~SplineInterpolate();

    void generateSpline(const std::vector<double> &x);

    double getValue(const double &s);

    bool interpolate(const std::vector<double> &base_index,
                     const std::vector<double> &base_value,
                     const std::vector<double> &return_index,
                     std::vector<double> &return_value);

    void getValueVector(const std::vector<double> &s_v,
                        std::vector<double> &value_v);

};

}    // namespace

#endif //LONGITUDINAL_CONTROLLER_BASE_HPP
