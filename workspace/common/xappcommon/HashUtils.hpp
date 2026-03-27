//
// Created by yixingwang on 2023/11/22.
//

#ifndef HASH_UTILS_HPP
#define HASH_UTILS_HPP

#include <openssl/md5.h>

#include <cstring>
#include <fstream>
#include <string>

namespace app_common {
    class HashUtils {
    public:
        HashUtils() = delete;

        static std::string MD5(const std::string& src) {
            MD5_CTX       ctx;
            std::string   md5_str;
            unsigned char md[16] = {0};
            char          tmp[33] = {0};
            MD5_Init(&ctx);
            MD5_Update(&ctx, src.c_str(), src.size());
            MD5_Final(md, &ctx);
            for (size_t i = 0; i < 16; ++i) {
                memset(tmp, 0x00, sizeof(tmp));
                sprintf(tmp, "%02x", md[i]);
                md5_str += tmp;
            }
            return md5_str;
        }

        static std::string MD5FromFile(const std::string& file_path) {
            const size_t  buffer_size = 4096;
            unsigned char buffer[buffer_size];
            unsigned char md[16] = {0};
            MD5_CTX       ctx;
            char          tmp[3] = {0};
            std::string   md5_str;

            std::ifstream file(file_path, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file: " + file_path);
            }

            MD5_Init(&ctx);
            while (file.good()) {
                file.read(reinterpret_cast<char*>(buffer), buffer_size);
                std::streamsize bytes_read = file.gcount();
                if (bytes_read > 0) {
                    MD5_Update(&ctx, buffer, bytes_read);
                }
            }

            MD5_Final(md, &ctx);

            for (size_t i = 0; i < 16; ++i) {
                sprintf(tmp, "%02x", md[i]);
                md5_str += tmp;
            }

            return md5_str;
        }
    };
}; // namespace app_common
#endif
