#ifndef RTSPSERVER2_SIMPLEFILEBUFFER_HPP
#define RTSPSERVER2_SIMPLEFILEBUFFER_HPP

#include <unistd.h>

#include "SimpleBuffer.hpp"

namespace app_common {

    class SimpleFileBuffer : public SimpleBuffer {
    public:
        explicit SimpleFileBuffer(std::size_t n) : SimpleBuffer(n), _touched_len(0) {
        }

        std::int64_t read_n(std::int32_t fd, std::size_t n) {
            resize(len() + n);

            auto r = read(fd, data() + len(), n);
            if (r < 0) {
                throw std::runtime_error(strerror(errno));
            }

            len_add(r);
            return r;
        }

        std::pair<std::uint8_t *, std::size_t> untouched_data() {
            if (_touched_len > len()) {
                throw std::runtime_error("error");
            }
            return {data() + _touched_len, len() - _touched_len};
        }

        const std::uint8_t *untouched() {
            return data() + _touched_len;
        }

        std::size_t untouched_len() {
            return len() - _touched_len;
        }

        void touch(std::size_t len) {
            _touched_len += len;
            if (_touched_len >= this->len()) {
                clear();
                _touched_len = 0;
            }
        }

        void clear() override {
            SimpleBuffer::clear();
            _touched_len = 0;
        }

        void reset() {
            SimpleFileBuffer::clear();
        }

    private:
        std::size_t _touched_len;
    };
} // namespace app_common
#endif
