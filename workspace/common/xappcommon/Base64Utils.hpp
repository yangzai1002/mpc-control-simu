#ifndef BASE64_UTILS_HPP
#define BASE64_UTILS_HPP

#include <algorithm>
#include <array>
#include <cctype>
#include <stdexcept>
#include <string>
#include <vector>

namespace app_common {
    class Base64Utils {
    public:
        Base64Utils() = delete;

        template <typename T> static std::string base64_encode_template(T begin, T end) {
            std::string result;
            size_t      size = std::distance(begin, end);
            result.reserve(((size + 2) / 3) * 4);

            while (begin != end) {
                std::array<uint8_t, 3> buf = {0};
                int                    buf_len = 0;

                for (; buf_len < 3 && begin != end; ++buf_len, ++begin) {
                    buf[buf_len] = *begin;
                }

                result += base64_chars[(buf[0] & 0xFC) >> 2];
                result += base64_chars[((buf[0] & 0x03) << 4) | ((buf[1] & 0xF0) >> 4)];
                result += (buf_len > 1 ? base64_chars[((buf[1] & 0x0F) << 2) | ((buf[2] & 0xC0) >> 6)] : '=');
                result += (buf_len > 2 ? base64_chars[buf[2] & 0x3F] : '=');
            }

            return result;
        }

        static std::string base64_encode(const std::string& str) {
            return base64_encode_template(str.begin(), str.end());
        }

        static std::string base64_encode(const std::vector<uint8_t>& data) {
            return base64_encode_template(data.begin(), data.end());
        }

        static std::vector<unsigned char> base64_decode(const std::string& encoded_string) {
            std::vector<unsigned char> ret;
            int                        i = 0;
            int                        j = 0;
            unsigned char              char_array_4[4], char_array_3[3];
            auto                       char_to_value = [](unsigned char c) -> unsigned char {
                switch (c) {
                    case 'A' ... 'Z':
                        return c - 'A';
                    case 'a' ... 'z':
                        return c - 'a' + 26;
                    case '0' ... '9':
                        return c - '0' + 52;
                    case '+':
                        return 62;
                    case '/':
                        return 63;
                    default:
                        return 0;
                }
            };
            for (char c : encoded_string) {
                if (c == '=') {
                    break;
                }
                char_array_4[i++] = c;
                if (i == 4) {
                    for (i = 0; i < 4; i++) {
                        char_array_4[i] = char_to_value(char_array_4[i]);
                    }
                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
                    for (i = 0; i < 3; i++) {
                        ret.push_back(char_array_3[i]);
                    }
                    i = 0;
                }
            }
            if (i) {
                for (j = i; j < 4; j++) {
                    char_array_4[j] = 0;
                }
                for (j = 0; j < 4; j++) {
                    char_array_4[j] = char_to_value(char_array_4[j]);
                }
                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                for (j = 0; j < i - 1; j++) {
                    ret.push_back(char_array_3[j]);
                }
            }
            return ret;
        }

    private:
        static constexpr const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        static inline bool is_base64(unsigned char c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }
    };
} // namespace app_common
#endif