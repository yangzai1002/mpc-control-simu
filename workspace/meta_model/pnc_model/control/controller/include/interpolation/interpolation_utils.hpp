#ifndef INTERPOLATION__INTERPOLATION_UTILS_HPP_
#define INTERPOLATION__INTERPOLATION_UTILS_HPP_


#include <stdexcept>
#include <vector>
#include <iostream>

namespace interpolation_utils
{

inline std::string to_string(int value) {
        char buf[32];
        sprintf(buf, "%d", value);

        return std::string(buf);
    }
//
inline bool isIncreasing(const std::vector<double> &x)
{
    if (x.empty())
    {
        throw std::invalid_argument("Points is empty.");
    }

    for (size_t i = 0; i < x.size() - 1; ++i)
    {
        if (x.at(i) >= x.at(i + 1))
        {
            return false;
        }
    }

    return true;
}


//
inline bool isNotDecreasing(const std::vector<double> &x)
{
    if (x.empty())
    {
        throw std::invalid_argument("Points is empty.");
    }

    for (size_t i = 0; i < x.size() - 1; ++i)
    {
        if (x.at(i) > x.at(i + 1))
        {
            return false;
        }
    }

    return true;
}


//
inline std::vector<double> validateKeys(const std::vector<double> &base_keys,
        const std::vector<double> &query_keys)
{
    if (base_keys.empty() || query_keys.empty())
    {
        throw std::invalid_argument("Points is empty.");
    }

    if (base_keys.size() < 2)
    {
        throw std::invalid_argument(
                "The size of points is less than 2. base_keys.size() = "
                        + to_string(base_keys.size()));
    }

    if (!isIncreasing(base_keys) || !isNotDecreasing(query_keys))
    {
        throw std::invalid_argument(
                "Either base_keys or query_keys is not sorted.");
    }

    constexpr double epsilon = 1e-3;
    if (query_keys.front() < base_keys.front() - epsilon
            || base_keys.back() + epsilon < query_keys.back())
    {
        throw std::invalid_argument("query_keys is out of base_keys");
    }

    auto validated_query_keys = query_keys;
    validated_query_keys.front() = std::max(validated_query_keys.front(),
            base_keys.front());
    validated_query_keys.back() = std::min(validated_query_keys.back(),
            base_keys.back());

    return validated_query_keys;
}


//
template<class T>
void validateKeysAndValues(const std::vector<double> &base_keys,
        const std::vector<T> &base_values)
{
    if (base_keys.empty() || base_values.empty())
    {
        throw std::invalid_argument("Points is empty.");
    }

    if (base_keys.size() < 2 || base_values.size() < 2)
    {
        throw std::invalid_argument(
                "The size of points is less than 2. base_keys.size() = "
                        + to_string(base_keys.size())
                        + ", base_values.size() = "
                        + to_string(base_values.size()));
    }

    if (base_keys.size() != base_values.size())
    {
        throw std::invalid_argument(
                "The size of base_keys and base_values are not the same.");
    }
}



}  // namespace

#endif  // INTERPOLATION__INTERPOLATION_UTILS_HPP_
