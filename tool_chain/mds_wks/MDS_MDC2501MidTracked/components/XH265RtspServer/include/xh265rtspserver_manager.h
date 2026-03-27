/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_XH265RTSPSERVER_XH265RTSPSERVER_MANAGER_H
#define ARA_XH265RTSPSERVER_XH265RTSPSERVER_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace xh265rtspserver {
class Xh265rtspserverManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    Xh265rtspserverManager() = default;
    virtual ~Xh265rtspserverManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace xh265rtspserver */
} /* namespace mdc */
#endif /* ARA_XH265RTSPSERVER_XH265RTSPSERVER_MANAGER_H */