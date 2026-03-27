
#include "interpolation/linear_interpolation.hpp"

namespace interpolation
{


double lerp(const double src_val, const double dst_val, const double ratio)
{
    return src_val + (dst_val - src_val) * ratio;
}


std::vector<double> lerp(const std::vector<double> &base_keys,
                         const std::vector<double> &base_values,
                         const std::vector<double> &query_keys)
{
    const auto validated_query_keys = interpolation_utils::validateKeys(
            base_keys, query_keys);
    interpolation_utils::validateKeysAndValues(base_keys, base_values);

    std::vector<double> query_values;
    size_t key_index = 0;
    for (const auto query_key : validated_query_keys)
    {
        while (base_keys.at(key_index + 1) < query_key)
        {
            ++key_index;
        }

        const double src_val = base_values.at(key_index);
        const double dst_val = base_values.at(key_index + 1);
        const double ratio = (query_key - base_keys.at(key_index))
                / (base_keys.at(key_index + 1) - base_keys.at(key_index));

        const double interpolated_val = lerp(src_val, dst_val, ratio);
        query_values.push_back(interpolated_val);
    }

    return query_values;
}



double lerp(const std::vector<double> &base_keys,
            const std::vector<double> &base_values, double query_key)
{
    return lerp(base_keys, base_values, std::vector<double>
    { query_key }).front();
}


}    // namespace interpolation
