#ifndef NET_POLLER_POLLERDESC_HPP
#define NET_POLLER_POLLERDESC_HPP

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

class PollerDesc {
public:
    enum class PollerEventType : std::uint32_t {
        EVENT_NONE = 0,
        EVENT_READ = 1,
        EVENT_WRITE = 2,
        EVENT_ERROR = 4,

        EVENT_RDEX = EVENT_READ | EVENT_ERROR,
        EVENT_ALL = EVENT_READ | EVENT_WRITE | EVENT_ERROR
    };

    // clang-format off
    explicit PollerDesc(
        std::int32_t fd
        , std::optional<std::function<void()>> cb_read =std::nullopt 
        , std::optional<std::function<void()>> cb_write=std::nullopt  
        , std::optional<std::function<void()>> cb_error =std::nullopt)
        : PollerDesc(
            fd
            , (std::uint32_t)PollerEventType::EVENT_NONE
            , std::move(cb_read)
            , std::move(cb_write)
            , std::move(cb_error)) {
        // clang-format on
    }

    // clang-format off
    PollerDesc(std::int32_t fd, std::uint32_t interested_event,
               std::optional<std::function<void()>> cb_read = std::nullopt,
               std::optional<std::function<void()>> cb_write = std::nullopt,
               std::optional<std::function<void()>> cb_error = std::nullopt)
        : fd(fd)
        , _event_interested(interested_event)
        , _event_received((std::uint32_t)PollerEventType::EVENT_NONE)
        , callback_read(std::move(cb_read))
        , callback_write(std::move(cb_write))
        , callback_error(std::move(cb_error)) {
    };
    // clang-format on

    ~PollerDesc() = default;

    void validate() const noexcept(false) {
        if (fd < 0) {
            throw std::runtime_error("invalid epoll_fd " + std::to_string(fd));
        }
    }

    [[nodiscard]] inline int get_fd() const {
        return fd;
    }

    inline void add_received_event(PollerEventType e) {
        _event_received |= (std::int32_t)e;
    }

    inline void event_received(std::uint32_t event) {
        _event_received = event;
    }

    [[nodiscard]] inline std::uint32_t event_received() const {
        return _event_received;
    }

    [[nodiscard]] inline bool is_event_received_nothing() const {
        return _event_received == (std::uint32_t)PollerEventType::EVENT_NONE;
    }

    [[nodiscard]] inline std::uint32_t event_interested() const {
        return _event_interested;
    }

    inline void interest_in_read() {
        _event_interested |= (std::uint32_t)PollerEventType::EVENT_READ;
    }
    inline void interest_in_write() {
        _event_interested |= (std::uint32_t)PollerEventType::EVENT_WRITE;
    }
    inline void interest_in_error() {
        _event_interested |= (std::uint32_t)PollerEventType::EVENT_ERROR;
    }
    inline void discard_read() {
        _event_interested &= ~(std::uint32_t)PollerEventType::EVENT_READ;
    }
    inline void discard_write() {
        _event_interested &= ~(std::uint32_t)PollerEventType::EVENT_WRITE;
    }
    inline void discard_error() {
        _event_interested &= ~(std::uint32_t)PollerEventType::EVENT_ERROR;
    }

    [[nodiscard]] bool is_interested_in_nothing() const {
        return _event_interested == (std::uint32_t)PollerEventType::EVENT_NONE;
    }
    [[nodiscard]] bool is_interested_in_read() const {
        return (_event_interested & (std::uint32_t)PollerEventType::EVENT_READ) != 0;
    }
    [[nodiscard]] bool is_interested_in_write() const {
        return (_event_interested & (std::uint32_t)PollerEventType::EVENT_WRITE) != 0;
    }
    [[nodiscard]] bool is_interested_in_error() const {
        return (_event_interested & (std::uint32_t)PollerEventType::EVENT_ERROR) != 0;
    };

    void handle_events() {
        if (callback_read && (_event_received & (std::uint32_t)PollerEventType::EVENT_READ)) {
            (*callback_read)();
            _event_received &= ~(std::uint32_t)PollerEventType::EVENT_READ;
        }

        if (callback_write && (_event_received & (std::uint32_t)PollerEventType::EVENT_WRITE)) {
            (*callback_write)();
            _event_received &= ~(std::uint32_t)PollerEventType::EVENT_WRITE;
        }

        if (callback_error && (_event_received & (std::uint32_t)PollerEventType::EVENT_ERROR)) {
            (*callback_error)();
            _event_received &= ~(std::uint32_t)PollerEventType::EVENT_ERROR;
        }
    }

private:
    std::int32_t                         fd;
    std::uint32_t                        _event_interested;
    std::uint32_t                        _event_received;
    std::optional<std::function<void()>> callback_read;
    std::optional<std::function<void()>> callback_write;
    std::optional<std::function<void()>> callback_error;
};
#endif
