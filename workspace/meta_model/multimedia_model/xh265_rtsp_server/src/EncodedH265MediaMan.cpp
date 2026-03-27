#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>

#include <fstream>
#include <iostream>

#include "DatetimeUtil.hpp"
#include "Ec407.hpp"
#include "EncodedH265MediaMan.hpp"
#include "PerfIndicators.hpp"
#include "ap_log/ap_log_interface.h"
#include "fmt/format.h"
#include "network/rtp/Error.hpp"

EncodedH265MediaMan::EncodedH265MediaMan(
    std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>>> que)
    : MediaMan(1), _stopped_working(true), _data_queue(std::move(que)) {
}

EncodedH265MediaMan::~EncodedH265MediaMan() {
    stop_working();
}

void EncodedH265MediaMan::stream_start() {
    if (0 == receiver_count(TrackId::Track0)) {
        return;
    }
    start_working();
}

void EncodedH265MediaMan::stream_stop() {
    stop_working();
}

bool EncodedH265MediaMan::has_receiver() {
    return 0 != receiver_count(TrackId::Track0);
}

void EncodedH265MediaMan::new_frame(const std::shared_ptr<ara::adsfi::MsgImageData> &frame) {
    if (__glibc_unlikely(!frame)) {
        return;
    }
    if (_stopped_working) {
        ApInfo("h265_rtsp_server") << "ignore frame of time: "
                                   << app_common::DatetimeUtil::double_time_from_sec_nsec(frame->timestamp);
        return;
    }
    _data_queue->push(frame);
    ApInfo("h265_rtsp_server") << _data_queue->size() << " frames in que after new frame";
}

void EncodedH265MediaMan::loop_working() {
    std::shared_ptr<ara::adsfi::MsgImageData> frame;
    while (!_stopped_working) {
        if (0 == receiver_count(TrackId::Track0)) {
            // 删除队列里面的数据，因为这时候没人看，未编码的数据放在队列里面干等着没有任何意义
            while (_data_queue->try_pop(frame)) {
                ApInfo("h265_rtsp_server")
                    << fmt::format("ignore frame of time {:20.8f}",
                                   app_common::DatetimeUtil::double_time_from_sec_nsec(frame->timestamp));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

#if 0 // 这里的本意是，如果队列里面有多个图像，则丢弃多余的图像，以避免图像的延迟
        std::shared_ptr<ara::adsfi::IdpVideoEncodedFrame> tmp;
        while (_data_queue->try_move_pop(tmp)) {
            frame = std::move(tmp);
        }
        if (frame->frame_data.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }
#else
        if (!_data_queue->try_move_pop(frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }
#endif
        try {
            std::lock_guard<std::mutex> g(_track0->_senders_mutex);
            for (auto &s : _track0->_rtp_senders) {
                s.second->send(frame->rawData.data(), frame->dataSize);
            }
            ApInfo("h265_rtsp_server") << fmt::format(
                "send frame of time {:20.8f}", app_common::DatetimeUtil::double_time_from_sec_nsec(frame->timestamp));
            FaultHandle::FaultApi::Instance()->ResetFaultCode(_ERRORCODE_FORMAT);
        } catch (const ExceptionParseNalus &e) {
            FaultHandle::FaultApi::Instance()->SetFaultCode(_ERRORCODE_FORMAT);
        } catch (const std::runtime_error &e) {
            ApInfo("h265_rtsp_server") << fmt::format(
                "error send frame of time {:20.8f}, error: {}",
                app_common::DatetimeUtil::double_time_from_sec_nsec(frame->timestamp), e.what());
        }
    }
}

void EncodedH265MediaMan::start_working() {
    _stopped_working = false;
    _thread_working = std::thread([this]() {
        pthread_setname_np(pthread_self(), "h265_media_man");
        loop_working();
    });
}

void EncodedH265MediaMan::stop_working() {
    _stopped_working = true;
    if (_thread_working.joinable()) {
        _thread_working.join();
    }
}
