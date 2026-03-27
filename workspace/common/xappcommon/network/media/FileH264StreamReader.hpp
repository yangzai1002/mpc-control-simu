#ifndef MEDIA_FILE_H264_STREAM_READER_HPP
#define MEDIA_FILE_H264_STREAM_READER_HPP

#include <fcntl.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "H264StreamReader.hpp"
#include "SimpleFileBuffer.hpp"
#include "StringUtils.hpp"

class FileH264StreamReader : public H264StreamReader {
public:
    explicit FileH264StreamReader(const std::string &file) : _fd(-1), _buf(1024 * 50) {
        _fd = open(file.c_str(), O_RDWR);
        if (_fd < 0) {
            throw std::runtime_error("open error-" + app_common::StringUtils::str_error(errno));
        }
    }

    ~FileH264StreamReader() {
        if (_fd > 0) {
            close(_fd);
            _fd = -1;
        }
    }

    void rewind() const {
        auto x = lseek(_fd, 0, SEEK_SET);
        if (x < 0) {
            throw std::runtime_error("lseek error-" + app_common::StringUtils::str_error(errno));
        }
    }

protected:
    std::int32_t                          _fd;
    app_common::SimpleFileBuffer _buf;
};

#endif
