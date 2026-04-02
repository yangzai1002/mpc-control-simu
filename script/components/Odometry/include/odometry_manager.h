/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_ODOMETRY_ODOMETRY_MANAGER_H
#define ARA_ODOMETRY_ODOMETRY_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace odometry {
class OdometryManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    OdometryManager() = default;
    virtual ~OdometryManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace odometry */
} /* namespace mdc */
#endif /* ARA_ODOMETRY_ODOMETRY_MANAGER_H */