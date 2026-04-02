/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_XDATATRIGGERRECORDER_XDATATRIGGERRECORDER_MANAGER_H
#define ARA_XDATATRIGGERRECORDER_XDATATRIGGERRECORDER_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace xdatatriggerrecorder {
class XdatatriggerrecorderManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    XdatatriggerrecorderManager() = default;
    virtual ~XdatatriggerrecorderManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace xdatatriggerrecorder */
} /* namespace mdc */
#endif /* ARA_XDATATRIGGERRECORDER_XDATATRIGGERRECORDER_MANAGER_H */