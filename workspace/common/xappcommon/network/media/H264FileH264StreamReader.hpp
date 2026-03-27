#ifndef MEDIA_H264_FILE_H264_STREAM_READER_HPP
#define MEDIA_H264_FILE_H264_STREAM_READER_HPP

#include <string>
#include <vector>

#include "FileH264StreamReader.hpp"
#include "SimpleFileBuffer.hpp"

class H264FileH264StreamReader : public FileH264StreamReader {
public:
    explicit H264FileH264StreamReader(const std::string &file)
        : FileH264StreamReader(file), _nalu_start(nullptr), _nalu_len(0), _nalu_total_len(0) {
    }

    ~H264FileH264StreamReader() {
    }

    std::pair<const std::uint8_t *, std::size_t> next_nalu() override {
        if (0 == _nalu_total_len || 0 == _buf.len()) {
            auto r = _buf.read_n(_fd, 1024);
            if (0 == r) {
                _nalu_start = nullptr;
                _nalu_len = 0;
                _nalu_total_len = 0;
                return {_nalu_start, _nalu_len}; // 空文件
            }
        } else {
            _buf.touch(_nalu_total_len);
        }

        auto d = _buf.untouched_data();
        auto first_start_code = start_code(d.first, d.second);
        if (!first_start_code.first) {
            if (0 == _nalu_total_len) {
                throw std::runtime_error("file format error");
            } else {
                _nalu_start = nullptr;
                _nalu_len = 0;
                _nalu_total_len = 0;
                return {_nalu_start, _nalu_len}; // 文件结束
            }
        }

        std::size_t offset = first_start_code.second;
        while (true) {
            d = _buf.untouched_data();
            auto next_start_code = find_next_start_code(d.first + offset, d.second - offset);
            if (next_start_code.first) {
                _nalu_start = d.first + first_start_code.second;
                _nalu_len = offset + next_start_code.second - first_start_code.second;
                _nalu_total_len = _nalu_len + first_start_code.second;
                return {_nalu_start, _nalu_len};
            }
            offset += next_start_code.second;

            auto r = _buf.read_n(_fd, 1024);
            if (0 == r) {
                d = _buf.untouched_data();
                _nalu_start = d.first + first_start_code.second;
                _nalu_len = d.second - first_start_code.second;
                _nalu_total_len = d.second;
                return {_nalu_start, _nalu_len};
            }

            // 为什么要回退几个字节呢？这是要处理当前读取的内容中已经包含了 start_code 的一部分
            auto x = offset - first_start_code.second;
            if (x > 4) {
                offset -= 5;
            } else if (x > 3) {
                offset -= 4;
            } else {
                offset = first_start_code.second;
            }
        }
    }

    void reset() override {
        _buf.clear();
    }

private:
    static bool start_code_3(const std::uint8_t *buf, std::size_t len) {
        if (len < 3) {
            return false;
        }

        return (buf[0] == 0 && buf[1] == 0 && buf[2] == 1);
    }

    static bool start_code_4(const std::uint8_t *buf, std::size_t len) {
        if (len < 4) {
            return false;
        }

        return (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1);
    }

    // .first: 是否是 start_code
    // .second: start_code 的长度
    static std::pair<bool, std::size_t> start_code(const std::uint8_t *buf, std::size_t len) {
        if (start_code_3(buf, len)) {
            return {true, 3};
        }

        if (start_code_4(buf, len)) {
            return {true, 4};
        }

        return {false, len};
    }

    // .first: 是否找到了
    // .second: start_code 的第 0 个字节的索引
    static std::pair<bool, std::size_t> find_next_start_code(const std::uint8_t *buf, std::size_t len) {
        std::size_t x = 0;

        while (x < len) {
            auto s = start_code(buf + x, len - x);
            if (s.first) {
                return {true, x};
            }
            ++x;
        }

        return {false, len};
    }

private:
    const std::uint8_t *_nalu_start;     // 不包含 stream_start code
    std::size_t         _nalu_len;       // 不包含 stream_start code
    std::size_t         _nalu_total_len; // 包含 stream_start code
};

#endif
