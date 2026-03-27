#ifndef VISIONPERCEPTION_H
#define VISIONPERCEPTION_H
#include <stdio.h>
#include <string.h>
#include <string>
namespace avos {
namespace perception {
inline std::string RLECompress(uint8_t *input, int size) {
        std::string compressed;
        int count = 1;
        uchar c;
        for (int i = 0; i < size - 1; i++) {
            c = input[i];
            if (input[i] == input[i + 1]) {
                count++;
            } else {
                for (int k = 0; k < sizeof(int); k++) {
                    uchar byte = (count >> (8 * k));
                    compressed += byte;
                }
                compressed += input[i];
                count = 1;
            }
        }
        for (int k = 0; k < sizeof(int); k++) {
            uchar byte = (count >> (8 * k));
            compressed += byte;
        }
        compressed += c;
        return compressed;
    }

    inline void RLEDecompress(const std::vector<uchar> &input,
                              std::vector<uchar> &output, int output_size) {
        output.clear();
        output.resize(output_size);
        uchar *data = output.data();
        for (int i = 0; i < input.size();) {
            int count = 0;
            for (int k = 0; k < sizeof(int); k++) {
                uchar byte = input[i + k];
                count |= (byte << (8 * k));
            }
            // std::vector<uchar> tmp(count, input[i + sizeof(int)]);
            // ret.insert(ret.end(), tmp.begin(), tmp.end());
            memset(data, input[i + sizeof(int)], count);
            data += count;

            i += 1 + sizeof(int);
        }
    }
}  // namespace perception
}  // namespace avos
#endif