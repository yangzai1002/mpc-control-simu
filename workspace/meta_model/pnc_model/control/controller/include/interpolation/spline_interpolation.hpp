#ifndef INTERPOLATION__SPLINE_INTERPOLATION_HPP_
#define INTERPOLATION__SPLINE_INTERPOLATION_HPP_


#include "interpolation/interpolation_utils.hpp"

#include <cmath>


namespace interpolation
{



// X(s) = a_i (s - s_i)^3 + b_i (s - s_i)^2 + c_i (s - s_i) + d_i : (i = 0, 1, ... N-1)
struct MultiSplineCoef
{
    MultiSplineCoef() = default;

    explicit MultiSplineCoef(const size_t num_spline)
    {
        a.resize(num_spline);
        b.resize(num_spline);
        c.resize(num_spline);
        d.resize(num_spline);
    }

    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> c;
    std::vector<double> d;
};



// static spline interpolation functions
std::vector<double> spline(const std::vector<double> &base_keys,
        const std::vector<double> &base_values,
        const std::vector<double> &query_keys);


//
std::vector<double> splineByAkima(const std::vector<double> &base_keys,
        const std::vector<double> &base_values,
        const std::vector<double> &query_keys);


}  // namespace interpolation


class SplineInterpolation
{
public:
    SplineInterpolation() = default;

    SplineInterpolation(const std::vector<double> &base_keys,
            const std::vector<double> &base_values)
    {
        calcSplineCoefficients(base_keys, base_values);
    }



    std::vector<double> getSplineInterpolatedValues(
            const std::vector<double> &query_keys) const;


    std::vector<double> getSplineInterpolatedDiffValues(
            const std::vector<double> &query_keys) const;


    std::vector<double> getSplineInterpolatedQuadDiffValues(
            const std::vector<double> &query_keys) const;


    size_t getSize() const
    {
        return base_keys_.size();
    }


private:
    std::vector<double> base_keys_;

    interpolation::MultiSplineCoef multi_spline_coef_;

    void calcSplineCoefficients(const std::vector<double> &base_keys,
            const std::vector<double> &base_values);


};

#endif  // INTERPOLATION__SPLINE_INTERPOLATION_HPP_
