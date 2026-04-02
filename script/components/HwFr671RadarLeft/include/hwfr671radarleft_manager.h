/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_HWFR671RADARLEFT_HWFR671RADARLEFT_MANAGER_H
#define ARA_HWFR671RADARLEFT_HWFR671RADARLEFT_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace hwfr671radarleft {
class Hwfr671radarleftManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    Hwfr671radarleftManager() = default;
    virtual ~Hwfr671radarleftManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace hwfr671radarleft */
} /* namespace mdc */
#endif /* ARA_HWFR671RADARLEFT_HWFR671RADARLEFT_MANAGER_H */