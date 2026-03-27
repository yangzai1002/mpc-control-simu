
#ifndef SIMPLE_BUFFER_HPP
#define SIMPLE_BUFFER_HPP

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>

#include "StringUtils.hpp"

namespace app_common {
    class SimpleBuffer {
    public:
        SimpleBuffer() : _p(nullptr), _capacity(0), _length(0) {
        }

        explicit SimpleBuffer(std::size_t n) : _capacity(n), _length(0) {
            _p = static_cast<uint8_t*>(malloc(n));
            if (nullptr == _p) {
                throw std::runtime_error("malloc error: " + StringUtils::str_error(errno));
            }
        }

        SimpleBuffer(const SimpleBuffer& b) {
            if (b._p != nullptr) {
                _p = static_cast<uint8_t*>(malloc(b._capacity));
                if (nullptr == _p) {
                    throw std::runtime_error("malloc error: " + StringUtils::str_error(errno));
                }
                memcpy(_p, b._p, b._length);
            } else {
                _p = nullptr;
            }

            _capacity = b._capacity;
            _length = b._length;
        }

        SimpleBuffer(const std::initializer_list<std::uint8_t>& il) : SimpleBuffer(il.size()) {
            memcpy(_p, il.begin(), il.size());
            _capacity = _length = il.size();
        }

        SimpleBuffer(SimpleBuffer&& b) noexcept {
            _p = b._p;
            _capacity = b._capacity;
            _length = b._length;

            b._p = nullptr;
            b._capacity = 0;
            b._length = 0;
        };

        ~SimpleBuffer() {
            if (_p != nullptr) {
                free(_p);
                _p = nullptr;
            }
            _capacity = 0;
            _length = 0;
        }

        virtual void clear() {
            _length = 0;
        }

        [[nodiscard]] std::uint8_t back() const {
            if (empty()) {
                throw std::runtime_error("empty");
            }

            return _p[_length - 1];
        }

        [[nodiscard]] std::uint8_t front() const {
            if (empty()) {
                throw std::runtime_error("empty");
            }

            return _p[0];
        };

        std::uint8_t& operator[](std::size_t i) {
            if (i >= _length) {
                throw std::out_of_range("");
            }

            return _p[i];
        }

        void set_uint8(std::uint32_t off, std::uint8_t v) {
            if (off >= _length) {
                throw std::out_of_range("");
            }
            memcpy(_p + off, &v, sizeof(v));
        }

        void set_uint16(std::uint32_t off, std::uint16_t v) {
            if (off >= _length) {
                throw std::out_of_range("");
            }
            v = htobe16(v);
            memcpy(_p + off, &v, sizeof(v));
        }

        void append_uint8(std::uint8_t v) {
            resize(_length + sizeof(v));
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_int16(std::int16_t v) {
            resize(_length + sizeof(v));
            v = htobe16(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_uint16(std::uint16_t v) {
            resize(_length + sizeof(v));
            v = htobe16(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_int32(std::int32_t v) {
            resize(_length + sizeof(v));
            v = htobe32(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_uint32(std::uint32_t v) {
            resize(_length + sizeof(v));
            v = htobe32(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_int64(std::int64_t v) {
            resize(_length + sizeof(v));
            v = htobe64(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_float(float v) {
            resize(_length + sizeof(v));
            v = htobe32(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append_double(double v) {
            resize(_length + sizeof(v));
            v = htobe64(v);
            memcpy(_p + _length, &v, sizeof(v));
            _length += sizeof(v);
        }

        void append(const std::uint8_t* data, std::size_t size) {
            resize(_length + size);

            memcpy(_p + _length, data, size);
            _length += size;
        }

        void append_string(const std::string& s, bool mqtt_encode = true) {
            if (s.size() > std::numeric_limits<std::uint16_t>::max()) {
                throw std::runtime_error("string of length " + std::to_string(s.size()) + " is too long");
            }

            if (mqtt_encode) {
                std::uint16_t len = s.size();
                push_back(len >> 8u);
                push_back(len & 0xFFu);
            }
            append((std::uint8_t*)s.c_str(), s.size());
        }

        void push_back(std::uint8_t u) {
            resize(_length + 1);
            _p[_length] = u;
            ++_length;
        }

        void pop_front() {
            if (empty()) {
                throw std::runtime_error("empty");
            }
            memmove(_p, _p + 1, _length - 1);
            --_length;
        }

        void resize(std::size_t n) {
            if (_capacity > n) {
                return;
            }

            auto b = static_cast<std::uint8_t*>(realloc(_p, n));
            if (b == nullptr) {
                throw std::runtime_error("realloc error: " + StringUtils::str_error(errno));
            }

            _p = b;
            _capacity = n;
        }

        [[nodiscard]] std::uint8_t* data() {
            return _p;
        }

        [[nodiscard]] const std::uint8_t* data() const {
            return _p;
        }

        [[nodiscard]] std::size_t size() const {
            return _capacity;
        }

        [[nodiscard]] std::size_t len() const {
            return _length;
        }

        void len_add(std::size_t n) {
            if (_length + n > _capacity) {
                throw std::runtime_error("len add_error");
            }

            _length += n;
        }

        void capacity_add(std::size_t n) {
            resize(_capacity + n);
        }

        [[nodiscard]] bool empty() const {
            return nullptr == _p || 0 == _length;
        }

        [[nodiscard]] bool full() const {
            return _length >= _capacity;
        }

    protected:
        std::uint8_t* _p;
        std::size_t   _capacity; // 当前容量
        std::size_t   _length;   // 当前实际数据长度
    };
}; // namespace app_common

#endif
