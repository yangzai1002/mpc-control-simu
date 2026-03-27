
#ifndef SIMPLE_FD_BUFFER_HPP
#define SIMPLE_FD_BUFFER_HPP

#include <netinet/in.h>
#include <unistd.h>

#include <cstring>

#include "SimpleBuffer.hpp"
#include "StringUtils.hpp"

namespace app_common {
    class SimpleSocketBuffer : public SimpleBuffer {
    public:
        explicit SimpleSocketBuffer(std::size_t n) : SimpleBuffer(n), _touched_len(0) {
        }

        std::int64_t read(std::int32_t fd) {
            if (full()) {
                resize(size() * 2);
            }

            auto n = ::read(fd, data() + len(), size() - len());
            if (n < 0) {
                throw std::runtime_error("read error with " + StringUtils::str_error(errno));
            }

            len_add(n);
            return n;
        }

        std::int64_t poll_and_read(std::int32_t fd, time_t timeout_in_ms = 100) {
            fd_set rd{};
            FD_SET(fd, &rd);
            timeval tv{.tv_sec = 0, .tv_usec = timeout_in_ms * 1000};

            auto o = select(fd + 1, &rd, nullptr, nullptr, &tv);
            if (o > 0) {
                return read(fd);
            }
            return 0;
        }

        std::int64_t recvfrom(std::int32_t fd, sockaddr_in& addr) {
            if (full()) {
                resize(size() * 2);
            }

            socklen_t socklen = sizeof(addr);
            auto n = ::recvfrom(fd, data() + len(), size() - len(), 0, reinterpret_cast<sockaddr*>(&addr), &socklen);
            if (n < 0) {
                throw std::runtime_error("read error-" + StringUtils::str_error(errno));
            }

            len_add(n);
            return n;
        }

        std::int64_t poll_and_read(std::int32_t fd, sockaddr_in& addr, time_t timeout_in_ms = 100) {
            fd_set rd{};
            FD_SET(fd, &rd);
            timeval tv{.tv_sec = 0, .tv_usec = timeout_in_ms * 1000};

            auto o = select(fd + 1, &rd, nullptr, nullptr, &tv);
            if (o > 0) {
                return recvfrom(fd, addr);
            }
            return 0;
        }

        std::pair<std::uint8_t*, std::size_t> untouched_data() {
            if (_touched_len > len()) {
                throw std::runtime_error("error");
            }
            return {data() + _touched_len, len() - _touched_len};
        }

        void touch(std::size_t len) {
            _touched_len += len;
            if (_touched_len >= this->len()) {
                clear();
                _touched_len = 0;
            }
        }

        void tidy() {
            memmove(_p, _p + _touched_len, _length - _touched_len);
            _length -= _touched_len;
            _touched_len = 0;
        }

        void clear() override {
            SimpleBuffer::clear();
            _touched_len = 0;
        }

        void reset() {
            SimpleSocketBuffer::clear();
        }

    private:
        std::size_t _touched_len;
    };
};     // namespace app_common
#endif // RTSPSERVER2_SIMPLEFDBUFFER_HPP
