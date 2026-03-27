#ifndef MEDIA_RECEIVER_HPP
#define MEDIA_RECEIVER_HPP

#include <cstdint>

class Receiver {
public:
    Receiver() : _error(0){};

    virtual ~Receiver() = default;

    [[nodiscard]] virtual std::string string() const = 0;

    [[nodiscard]] virtual std::int32_t error() const {
        return _error;
    }

    virtual void error(std::int32_t err) {
        _error = err;
    }

    virtual void push(const std::uint8_t *p, std::size_t s, std::int32_t arg) noexcept(false) = 0;

    virtual void push(const std::uint8_t *p1, std::size_t s1, const std::uint8_t *p2, std::size_t s2,
                      std::int32_t arg) noexcept(false) = 0;

protected:
    std::int32_t _error;
};

#endif
