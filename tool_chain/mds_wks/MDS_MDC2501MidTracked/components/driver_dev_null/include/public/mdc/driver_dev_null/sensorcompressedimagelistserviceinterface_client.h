/* *
 * Class: SensorCompressedImageListServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACECLIENT
#define ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msgimagedatalist.h"





namespace ara {
namespace adsfi {

using EventCompressedImageListHandlerType = std::function<void (ara::adsfi::MsgImageDataList)>;
using EventCompressedImageListDataType = ara::adsfi::MsgImageDataList;
using RecvEventCompressedImageListType = std::shared_ptr<EventCompressedImageListDataType>;
class SensorCompressedImageListServiceInterfaceClient final {
public:
    SensorCompressedImageListServiceInterfaceClient() = delete;
    explicit SensorCompressedImageListServiceInterfaceClient(const uint32_t instanceId);
    virtual ~SensorCompressedImageListServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventCompressedImageListNotifyHandler(const EventCompressedImageListHandlerType handler);
    void EventCompressedImageListNotify();
    void EventCompressedImageListContainerClear();
    bool EventCompressedImageListContainerEmpty();
    RecvEventCompressedImageListType GetEventCompressedImageListOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventCompressedImageListType GetEventCompressedImageListOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventCompressedImageListType> GetEventCompressedImageListNdata(const size_t n);

    uint32_t GetRecvQSize() const;
    bool IsStop() const;
    uint32_t GetInstanceId() const;

    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif