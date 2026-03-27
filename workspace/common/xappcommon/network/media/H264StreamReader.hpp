#ifndef RTP_H264_STREAM_READER_HPP
#define RTP_H264_STREAM_READER_HPP

#include <string>

class H264StreamReader {
public:
    virtual std::pair<const std::uint8_t*, std::size_t> next_nalu() = 0;

    virtual void reset() = 0;
};

#endif
