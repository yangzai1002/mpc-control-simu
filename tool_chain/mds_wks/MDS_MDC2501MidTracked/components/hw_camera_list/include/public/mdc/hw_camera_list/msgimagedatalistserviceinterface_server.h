/* *
 * Class: MsgImageDataListServiceInterface server declaration
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * */
#ifndef ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACESERVER
#define ARA_ADSFI_MSGIMAGEDATALISTSERVICEINTERFACESERVER
#include <memory>

#include "ara/adsfi/impl_type_msgimagedatalist.h"





namespace ara {
namespace adsfi {

using EventImageListDataType = ara::adsfi::MsgImageDataList;
using SendEventImageListType = std::shared_ptr<EventImageListDataType>;


class MsgImageDataListServiceInterfaceServer final {
public:
    MsgImageDataListServiceInterfaceServer() = delete;

    explicit MsgImageDataListServiceInterfaceServer(const uint32_t id);

    virtual ~MsgImageDataListServiceInterfaceServer();

    bool Init();

    void Stop();

    bool IsStop() const;

    uint32_t GetInstanceId() const;

    /* event relative */
    bool SendEventImageListData(const SendEventImageListType& data);
    void ClearEventImageListContainer();
    
    

private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};
} /* namespace adsfi */
} /* namespace ara */

#endif