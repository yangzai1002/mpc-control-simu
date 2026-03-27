
#include "adsfi_interface.h"
#include "ap_log/ap_log_interface.h"
#include "config_loader/custom_stack.h"
#include "log/avos_node.h"

void AdsfiInterface::Init() {
    SetScheduleInfo("message");

    // ApLogInterface::Instance()->Init("XH265Recorder");
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgImageData> &ptr) {
    if (!_receiver) {
        try {
            _receiver = std::make_shared<H265ThreadedRotatingFileReceiver>();
        } catch (const std::exception &e) {
            ApInfo("h265_recorder") << "ERROR: " << e.what();
            return;
        }
    }
    if (__glibc_unlikely(!ptr)) {
        ApError("h265_recorder") << "frame is null";
        return;
    }

    if (__glibc_unlikely(0 == ptr->dataSize)) {
        ApError("h265_recorder") << "frame is empty";
        return;
    }

    if (__glibc_unlikely(ptr->dataSize > ptr->rawData.size())) {
        ApError("h265_recorder") << "frame size overflow, datasize: " << ptr->dataSize
                                 << ", rawdata_size: " << ptr->rawData.size();
        return;
    }

    if (__glibc_unlikely(ptr->imageType != 53)) {
        ApError("h265_recorder") << "frame type error, imagetype: " << static_cast<std::int32_t>(ptr->imageType);
        return;
    }

    _receiver->push(ptr->rawData.data(), ptr->dataSize);
    ApInfo("h265_recorder") << "received " << ptr->dataSize << " bytes ";
}
