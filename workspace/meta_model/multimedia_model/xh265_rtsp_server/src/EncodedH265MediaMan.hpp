#ifndef ENCODED_H264_MEDIA_MAN_HPP
#define ENCODED_H264_MEDIA_MAN_HPP

#include <mutex>
#include <thread>

#include "SafeQueue.hpp"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "network/media/MediaMan.hpp"

class EncodedH265MediaMan : public MediaMan {
public:
    EncodedH265MediaMan(std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>>> que);

    ~EncodedH265MediaMan() override;

    void stream_start() override;

    void stream_stop() override;

    bool has_receiver();

    void new_frame(const std::shared_ptr<ara::adsfi::MsgImageData> &frame);

private:
    std::atomic_bool                                                                  _stopped_working;
    std::thread                                                                       _thread_working;
    std::shared_ptr<app_common::SafeQueue<std::shared_ptr<ara::adsfi::MsgImageData>>> _data_queue;

protected:
    void loop_working();

    void start_working();

    void stop_working();
};

#endif
