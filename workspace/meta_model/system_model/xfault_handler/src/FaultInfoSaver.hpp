#ifndef _H264_RECEIVER_HPP_
#define _H264_RECEIVER_HPP_

#include <cstdint>
#include <string>

class FaultInfoSaver {
public:
    FaultInfoSaver() : _error(0){};

    virtual ~FaultInfoSaver() = default;

    [[nodiscard]] virtual std::string string() const = 0;

    [[nodiscard]] virtual std::int32_t error() const {
        return _error;
    }

    virtual void error(std::int32_t err) {
        _error = err;
    }

    virtual void push(const std::string& fault_record) noexcept(false) = 0;

protected:
    std::int32_t _error;
};

#endif
