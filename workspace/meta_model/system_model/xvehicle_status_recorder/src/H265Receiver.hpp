#ifndef _H265_RECEIVER_HPP_
#define _H265_RECEIVER_HPP_

#include <cstdint>
#include <string>

class H265Receiver {
public:
    H265Receiver() : _error(0){};

    virtual ~H265Receiver() = default;

    [[nodiscard]] virtual std::string string() const = 0;

    [[nodiscard]] virtual std::int32_t error() const {
        return _error;
    }

    virtual void error(std::int32_t err) {
        _error = err;
    }

    virtual void push(const std::uint8_t *p, std::size_t s) noexcept(false) = 0;

protected:
    std::int32_t _error;
};

#endif
