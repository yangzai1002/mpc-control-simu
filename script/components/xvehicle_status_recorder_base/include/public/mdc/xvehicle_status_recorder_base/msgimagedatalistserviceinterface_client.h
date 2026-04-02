/* *
 * Class: MsgImageDataListServiceInterface client declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */

#ifndef ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACECLIENT
#define ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACECLIENT

#include <memory>
#include <vector>
#include <atomic>

#include "ara/adsfi/impl_type_msgimagedatalist.h"





namespace ara {
namespace adsfi {

using EventImageListHandlerType = std::function<void (ara::adsfi::MsgImageDataList)>;
using EventImageListDataType = ara::adsfi::MsgImageDataList;
using RecvEventImageListType = std::shared_ptr<EventImageListDataType>;
class MsgImageDataListServiceInterfaceClient final {
public:
    MsgImageDataListServiceInterfaceClient() = delete;
    explicit MsgImageDataListServiceInterfaceClient(const uint32_t instanceId);
    virtual ~MsgImageDataListServiceInterfaceClient();
    bool Init();
    void Stop();

    /* event relative */
    void RegisterEventImageListNotifyHandler(const EventImageListHandlerType handler);
    void EventImageListNotify();
    void EventImageListContainerClear();
    bool EventImageListContainerEmpty();
    RecvEventImageListType GetEventImageListOneData(const uint32_t freshDataTime = UINT32_MAX);
    RecvEventImageListType GetEventImageListOneDataBlocking(const uint32_t blockTimeout = UINT32_MAX);
    std::vector<RecvEventImageListType> GetEventImageListNdata(const size_t n);

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