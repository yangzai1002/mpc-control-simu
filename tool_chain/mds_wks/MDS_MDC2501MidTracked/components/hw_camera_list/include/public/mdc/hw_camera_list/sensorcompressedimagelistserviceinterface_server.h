/* *
 * Class: SensorCompressedImageListServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACESERVER
#define ARA_ADSFI_SENSORCOMPRESSEDIMAGELISTSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msgimagedatalist.h"





namespace ara {
namespace adsfi {

using EventCompressedImageListDataType = ara::adsfi::MsgImageDataList;
using SendEventCompressedImageListType = std::shared_ptr<EventCompressedImageListDataType>;


class SensorCompressedImageListServiceInterfaceServer final {
public:
    SensorCompressedImageListServiceInterfaceServer() = delete;

    explicit SensorCompressedImageListServiceInterfaceServer(const uint32_t id);

    virtual ~SensorCompressedImageListServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventCompressedImageListData(const SendEventCompressedImageListType& data);
    void ClearEventCompressedImageListContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif