/* *
 * Class: VisionSegmentServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACESERVER
#define ARA_ADSFI_VISIONSEGMENTSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msgcameravpresult.h"





namespace ara {
namespace adsfi {

using EventVisionSegmentDataType = ara::adsfi::MsgCameraVPResult;
using SendEventVisionSegmentType = std::shared_ptr<EventVisionSegmentDataType>;


class VisionSegmentServiceInterfaceServer final {
public:
    VisionSegmentServiceInterfaceServer() = delete;

    explicit VisionSegmentServiceInterfaceServer(const uint32_t id);

    virtual ~VisionSegmentServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventVisionSegmentData(const SendEventVisionSegmentType& data);
    void ClearEventVisionSegmentContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif