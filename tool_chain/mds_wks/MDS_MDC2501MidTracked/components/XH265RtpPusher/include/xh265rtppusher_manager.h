/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_XH265RTPPUSHER_XH265RTPPUSHER_MANAGER_H
#define ARA_XH265RTPPUSHER_XH265RTPPUSHER_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace xh265rtppusher {
class Xh265rtppusherManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    Xh265rtppusherManager() = default;
    virtual ~Xh265rtppusherManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace xh265rtppusher */
} /* namespace mdc */
#endif /* ARA_XH265RTPPUSHER_XH265RTPPUSHER_MANAGER_H */