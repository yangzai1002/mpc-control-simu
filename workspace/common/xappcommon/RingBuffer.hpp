#ifndef HELMET_MCU_RINGBUFFER_HPP
#define HELMET_MCU_RINGBUFFER_HPP

#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace app_common {
    class RingBuffer {
    public:
        explicit RingBuffer(std::int64_t capacity) : front_(0), rear_(0), size_(0), capacity_(capacity) {
            if (capacity <= 0) {
                throw std::runtime_error("capacity <= 0");
            }

            data_ = new std::uint8_t[capacity];
        }

        RingBuffer(RingBuffer &&) = delete;

        RingBuffer &operator=(RingBuffer &&other) = delete;

        RingBuffer(const RingBuffer &) = delete;

        RingBuffer &operator=(const RingBuffer &other) = delete;

        ~RingBuffer() {
            delete[] data_;
        }

        void clear() {
            std::unique_lock<std::mutex> lock(mut_);

            front_ = rear_ = size_ = 0;
        }

        bool empty() {
            return 0 == size();
        }

        [[maybe_unused]] [[nodiscard]] std::int64_t size() {
            std::unique_lock<std::mutex> lock(mut_);
            return size_;
        }

        [[maybe_unused]] [[nodiscard]] std::int64_t capacity() const {
            return capacity_;
        }

        [[maybe_unused]] std::int64_t write(const std::uint8_t *data, std::int64_t bytes) {
            if (nullptr == data || bytes <= 0) {
                return 0;
            }
            if (capacity_ < bytes) {
                throw std::runtime_error("bytes > capacity");
            }

            std::unique_lock<std::mutex> lock(mut_);

            while ((capacity_ - size_) < bytes) {
                cond_writable_.wait_for(lock, std::chrono::milliseconds(50));
            }

            if (bytes <= capacity_ - rear_) {
                memcpy(data_ + rear_, data, bytes);
                rear_ += bytes;
                if (rear_ == capacity_) {
                    rear_ = 0;
                }
            } else {
                auto size_1 = capacity_ - rear_;
                memcpy(data_ + rear_, data, size_1);

                auto size_2 = bytes - size_1;
                memcpy(data_, data + size_1, size_2);
                rear_ = size_2;
            }

            size_ += bytes;
            cond_readable_.notify_all();

            return bytes;
        }

        [[maybe_unused]] std::int64_t read(std::uint8_t *data, std::int64_t bytes) {
            if (bytes <= 0) {
                return bytes;
            }
            if (capacity_ < bytes) {
                throw std::runtime_error("capacity < bytes");
            }

            std::unique_lock<std::mutex> lock(mut_);

            while (size_ < bytes) {
                cond_readable_.wait_for(lock, std::chrono::milliseconds(50));
            }

            if (bytes <= (capacity_ - front_)) {
                memcpy(data, data_ + front_, bytes);
                front_ += bytes;
                if (front_ == capacity_) {
                    front_ = 0;
                }
            } else {
                const auto size_1 = capacity_ - front_;
                memcpy(data, data_ + front_, size_1);

                const auto size_2 = bytes - size_1;
                memcpy(data + size_1, data_, size_2);
                front_ = size_2;
            }

            size_ -= bytes;
            cond_writable_.notify_all();

            return bytes;
        }

        [[maybe_unused]] std::int64_t read(std::uint8_t *data, std::int64_t bytes, std::int64_t bytes_align) {
            if (bytes <= 0) {
                return bytes;
            }
            if (capacity_ < bytes) {
                throw std::runtime_error("capacity < bytes");
            }
            if (bytes_align <= 0) {
                throw std::runtime_error("bytes_align <= 0");
            }

            std::unique_lock<std::mutex> lock(mut_);

            std::int64_t readable = 0;
            while (0 == size_) {
                cond_readable_.wait_for(lock, std::chrono::milliseconds(50));
                if (0 == size_) {
                    return 0;
                }
            }

            if (size_ < bytes) {
                auto produce = size_ / bytes_align;
                readable = produce * bytes_align;
            } else {
                auto produce = bytes / bytes_align;
                readable = produce * bytes_align;
            }

            if (readable <= (capacity_ - front_)) {
                memcpy(data, data_ + front_, readable);
                front_ += readable;
                if (front_ == capacity_) {
                    front_ = 0;
                }
            } else {
                const auto size_1 = capacity_ - front_;
                memcpy(data, data_ + front_, size_1);

                const auto size_2 = readable - size_1;
                memcpy(data + size_1, data_, size_2);
                front_ = size_2;
            }

            size_ -= readable;
            cond_writable_.notify_all();

            return readable;
        }

    private:
        const std::int64_t      capacity_;
        std::int64_t            front_, rear_, size_;
        uint8_t *               data_;
        std::mutex              mut_;
        std::condition_variable cond_writable_;
        std::condition_variable cond_readable_;
    };
} // namespace app_common

#endif // HELMET_MCU_RINGBUFFER_HPP
