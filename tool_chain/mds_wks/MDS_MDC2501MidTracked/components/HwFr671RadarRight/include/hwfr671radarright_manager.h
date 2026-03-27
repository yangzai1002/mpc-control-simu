/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_HWFR671RADARRIGHT_HWFR671RADARRIGHT_MANAGER_H
#define ARA_HWFR671RADARRIGHT_HWFR671RADARRIGHT_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace hwfr671radarright {
class Hwfr671radarrightManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    Hwfr671radarrightManager() = default;
    virtual ~Hwfr671radarrightManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace hwfr671radarright */
} /* namespace mdc */
#endif /* ARA_HWFR671RADARRIGHT_HWFR671RADARRIGHT_MANAGER_H */