#ifndef AVOS_COMMON_UTILS_RANDOMUTIL_HPP
#define AVOS_COMMON_UTILS_RANDOMUTIL_HPP

#include <string>

static constexpr const char charset[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

namespace app_common {
    class RandomUtil {
    public:
        RandomUtil() = delete;

        static std::string strN(std::uint32_t n) {
            const size_t size = sizeof(charset) - 1;

            auto randomChar = [&]() { return charset[rand() % size]; };

            std::string result(n, 0);
            std::generate_n(result.begin(), n, randomChar);
            return result;
        }

    private:
    };

} // namespace app_common
#endif
