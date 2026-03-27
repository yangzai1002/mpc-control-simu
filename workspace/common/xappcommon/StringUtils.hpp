#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <arpa/inet.h>
#include <netinet/in.h>

#include <algorithm>
#include <cstring>
#include <string>

namespace app_common {
    class StringUtils {
    public:
        StringUtils() = delete;

        [[maybe_unused]] static std::string inet_ntoa(std::uint32_t a) {
            char buff[INET_ADDRSTRLEN];

            return inet_ntop(AF_INET, &a, buff, sizeof(buff));
        }

        [[maybe_unused]] static std::string str_error(std::int32_t err) {
            char buff[256]{};
            return std::to_string(err) + ":" + strerror_r(err, buff, sizeof(buff) - 1);
        }

        [[maybe_unused]] static bool starts_with(const std::string& s, const std::string& w) {
            if (s.size() < w.size()) {
                return false;
            }

            return s.find(w) != std::string::npos;
        }

        [[maybe_unused]] static bool ends_with(const std::string& s, const std::string& m) {
            if (s.size() < m.size()) {
                return false;
            }

            auto sub = s.substr(s.size() - m.size());
            return sub == m;
        }

        [[maybe_unused]] static std::string to_lower(std::string str) {
            std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
            return str;
        }

        [[maybe_unused]] static std::string to_upper(std::string str) {
            std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
            return str;
        }

        static void trim_left(std::string& s) {
            auto pos = s.find_first_not_of(' ');
            if (pos != std::string::npos) {
                s.erase(0, pos);
            } else {
                s.clear();
            }
        }

        static void trim_right(std::string& s) {
            auto pos = s.find_last_not_of(' ');
            if (pos != std::string::npos) {
                s.erase(pos + 1);
            } else {
                s.clear();
            }
        }

        static std::string trim_left(const std::string& x) {
            std::string v(x);
            trim_left(v);
            return v;
        }

        static std::string trim_right(const std::string& x) {
            std::string v(x);
            trim_right(v);
            return v;
        }

        static std::string trim(const std::string& s) {
            std::string v(s);

            trim_left(v);
            trim_right(v);

            return v;
        }

        static void trim(std::string& s) {
            trim_left(s);
            trim_right(s);
        }

        template <typename Container> static std::string join(const Container& l, const std::string& separator) {
            std::string r;
            for (const auto& s : l) {
                r.append(s);
                r.append(separator);
            }
            auto pos = r.rfind(separator);
            if (pos != std::string::npos) {
                r.erase(pos);
            }
            return r;
        }

        template <template <typename...> class Container>
        static Container<std::string> tokenize(char* s, const char* sep) {
            char* str = const_cast<char*>(s);
            char* saved = nullptr;

            Container<std::string> l;
            auto                   n = strtok_r(str, sep, &saved);
            while (n != nullptr) {
                l.emplace_back(n);
                n = strtok_r(nullptr, sep, &saved);
            }

            return l;
        }
    };
}; // namespace app_common
#endif
