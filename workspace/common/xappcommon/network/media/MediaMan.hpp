#ifndef MEDIA_MEDIAMAN_HPP
#define MEDIA_MEDIAMAN_HPP

#include <array>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "../net/InetAddress.hpp"
#include "../rtp/RtpReceiver.hpp"
#include "../rtp/RtpSender.hpp"
#include "NetUtils.hpp"
#include "PerfIndicators.hpp"
#include "Sender.hpp"
#include "TrackId.hpp"
#include "fmt/format.h"

class MediaMan {
public:
    struct Track {
        std::mutex                                     _senders_mutex;
        std::map<std::string, std::shared_ptr<Sender>> _rtp_senders;
    };

public:
    explicit MediaMan(std::uint32_t num_tracks)
        : _track_num(num_tracks), _track0(std::make_shared<Track>()), _track1(std::make_shared<Track>()) {
    }

    virtual ~MediaMan() = default;

public:
    virtual void stream_start() = 0;

    virtual void stream_stop() = 0;

    std::shared_ptr<Sender> sender(TrackId track, const std::string &sender_name) {
        auto p = TrackId::Track0 == track ? _track0 : _track1;
        if (!p) {
            throw std::runtime_error(fmt::format("track {} disabled", (std::int32_t)track));
        }
        try {
            return p->_rtp_senders.at(sender_name);
        } catch (const std::out_of_range &e) {
            throw std::runtime_error(fmt::format("sender {} invalid", sender_name));
        }
    }

    std::string sdp(TrackId track, const std::string &sender_name) {
        return sender(track, sender_name)->sdp();
    }

    std::string local_ip(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->local_ip();
    }

    std::uint16_t local_rtp_port(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->local_rtp_port();
    }

    std::uint16_t local_rtcp_port(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->local_rtcp_port();
    }

    bool is_multicast(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->is_multicast();
    }

    std::string mcast_ip(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->mcast_ip();
    }

    std::uint16_t mcast_rtp_port(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->mcast_rtp_port();
    }

    std::uint16_t mcast_rtcp_port(TrackId track, const std::string &sender_name) {
        auto s = sender(track, sender_name);
        auto rs = dynamic_cast<RtpSender *>(s.get());
        return rs->mcast_rtcp_port();
    }

    void add_sender(TrackId track, const std::string &sender_name, const std::shared_ptr<Sender> &sender) {
        auto p = TrackId::Track0 == track ? _track0 : _track1;
        if (!p) {
            throw std::runtime_error(fmt::format("track {} disabled", (std::int32_t)track));
        }

        auto old_count = receiver_count(TrackId::Track0) + receiver_count(TrackId::Track1);

        {
            std::lock_guard<std::mutex> g(p->_senders_mutex);
            p->_rtp_senders.emplace(sender_name, sender);
        }

        auto new_count = receiver_count(TrackId::Track0) + receiver_count(TrackId::Track1);
        if (0 == old_count && 1 == new_count) {
            try {
                stream_start();
            } catch (const std::runtime_error &e) {
                del_sender(track, sender_name);
                std::rethrow_exception(std::current_exception());
            }
        }
    }

    void del_sender(TrackId track, const std::string &sender_name) {
        auto p = TrackId::Track0 == track ? _track0 : _track1;
        if (!p) {
            throw std::runtime_error(fmt::format("track {} disabled", (std::int32_t)track));
        }

        {
            std::lock_guard<std::mutex> g(p->_senders_mutex);
            p->_rtp_senders.erase(sender_name);
        }

        if (0 == (receiver_count(TrackId::Track0) + receiver_count(TrackId::Track1))) {
            stream_stop();
        }
    }

    std::uint32_t receiver_count(TrackId track) {
        auto p = TrackId::Track0 == track ? _track0 : _track1;
        if (!p) {
            throw std::runtime_error(fmt::format("track {} disabled", (std::int32_t)track));
        }
        std::lock_guard<std::mutex> g(p->_senders_mutex);

        std::uint32_t t = 0;
        for (const auto &s : p->_rtp_senders) {
            t += s.second->receiver_count();
        }

        return t;
    }

    void dump_receivers(TrackId track) {
        auto p = TrackId::Track0 == track ? _track0 : _track1;
        if (!p) {
            throw std::runtime_error(fmt::format("track {} disabled", (std::int32_t)track));
        }
        std::lock_guard<std::mutex> g(p->_senders_mutex);

        for (const auto &x : p->_rtp_senders) {
            std::cout << fmt::format("----------------- track {} receivers of sender {} ------------------",
                                     (std::int32_t)track, x.first)
                      << std::endl;
            x.second->dump_receivers();
        }
    }

    void dump_receivers() {
        dump_receivers(TrackId::Track0);
        dump_receivers(TrackId::Track1);
    }

    void add_receiver(TrackId track, const std::string &sender_name, const std::string &receiver_name,
                      const std::shared_ptr<RtpReceiver> r) {
        auto old_count = receiver_count(TrackId::Track0) + receiver_count(TrackId::Track1);

        sender(track, sender_name)->add_receiver(receiver_name, r);

        auto new_count = receiver_count(TrackId::Track0) + receiver_count(TrackId::Track1);
        if (0 == old_count && 1 == new_count) {
            try {
                stream_start();
            } catch (const std::runtime_error &e) {
                sender(track, sender_name)->del_receiver(receiver_name);
                std::rethrow_exception(std::current_exception());
            }
        }
    }

    void del_receiver(TrackId track, const std::string &sender_name, const std::string &receiver_name) {
        sender(track, sender_name)->del_receiver(receiver_name);

        if (0 == (receiver_count(TrackId::Track0) + receiver_count(TrackId::Track1))) {
            stream_stop();
        }
    }

    void del_receiver(const std::string &sender_name, const std::string &receiver_name) {
        try {
            del_receiver(TrackId::Track0, sender_name, receiver_name);
        } catch (const std::runtime_error &e) {
            std::cout << fmt::format("track {}: del receiver {} from sender {} error: {}",
                                     (std::int32_t)TrackId::Track0, receiver_name, sender_name, e.what())
                      << std::endl;
        }

        try {
            del_receiver(TrackId::Track1, sender_name, receiver_name);
        } catch (const std::runtime_error &e) {
            std::cout << fmt::format("track {}: del receiver {} from sender {} error: {}",
                                     (std::int32_t)TrackId::Track1, receiver_name, sender_name, e.what())
                      << std::endl;
        }
    }

    [[nodiscard]] std::uint32_t tracks() const {
        return _track_num;
    }

protected:
    const std::uint32_t    _track_num;
    std::shared_ptr<Track> _track0;
    std::shared_ptr<Track> _track1;

public:
    class Perf {
    public:
        struct Stats {
            Stats()
                : avg_us_color_converting(0)
                , avg_us_scaling(0)
                , avg_us_encoding(0)
                , avg_us_sending(0)
                , avg_us_span_from_driver_to_scaling(0)
                , avg_us_span_from_driver_to_send_done(0) {
            }
            double avg_us_color_converting;
            double avg_us_scaling;
            double avg_us_encoding;
            double avg_us_sending;
            double avg_us_span_from_driver_to_scaling;
            double avg_us_span_from_driver_to_send_done;
        };

    public:
        void on_convert(std::int64_t color_converting) {
            std::lock_guard<std::mutex> lg(_mtx);
            us_color_converting.add(color_converting);
        }

        void on_scale(std::int64_t scaling, std::int64_t span) {
            std::lock_guard<std::mutex> lg(_mtx);
            us_scaling.add(scaling);
            us_span_from_driver_to_scaling.add(span);
        }

        void on_encode(std::int64_t encoding) {
            std::lock_guard<std::mutex> lg(_mtx);
            us_encoding.add(encoding);
        }

        void on_send(std::int64_t sending, std::int64_t span) {
            std::lock_guard<std::mutex> lg(_mtx);
            us_sending.add(sending);
            us_span_from_driver_to_send_done.add(span);
        }

        Stats stats() {
            std::lock_guard<std::mutex> lg(_mtx);
            Stats                       s{};

            s.avg_us_color_converting = us_color_converting.avg();
            s.avg_us_scaling = us_scaling.avg();
            s.avg_us_encoding = us_encoding.avg();
            s.avg_us_sending = us_sending.avg();
            s.avg_us_span_from_driver_to_scaling = us_span_from_driver_to_scaling.avg();
            s.avg_us_span_from_driver_to_send_done = us_span_from_driver_to_send_done.avg();

            return s;
        }

    private:
        std::mutex _mtx;

        app_common::TimeCostIndicator<500> us_color_converting;
        app_common::TimeCostIndicator<500> us_scaling;
        app_common::TimeCostIndicator<500> us_encoding;
        app_common::TimeCostIndicator<500> us_sending;
        app_common::TimeCostIndicator<500> us_span_from_driver_to_scaling;
        app_common::TimeCostIndicator<500> us_span_from_driver_to_send_done;
    };

    Perf _perf;

public:
    Perf::Stats stats() {
        return _perf.stats();
    }
};

#endif
