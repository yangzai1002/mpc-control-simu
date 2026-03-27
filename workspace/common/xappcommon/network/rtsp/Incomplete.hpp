#ifndef RTSP_INCOMPLETE_HPP
#define RTSP_INCOMPLETE_HPP

#include <stdexcept>

class Incomplete : public std::runtime_error {
public:
    Incomplete() : std::runtime_error("") {
    }
};

#endif
