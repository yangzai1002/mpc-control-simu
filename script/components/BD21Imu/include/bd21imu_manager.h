/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: MDC平台AP标准框架
 */
#ifndef ARA_BD21IMU_BD21IMU_MANAGER_H
#define ARA_BD21IMU_BD21IMU_MANAGER_H

#include "mdc/mdc_adaptive_application.h"
#include "ara/core/string.h"
#include "mdc/sample.h"
namespace mdc {
namespace bd21imu {
class Bd21imuManager : public MdcAdaptiveApplication {
public:
    /* 若需传入参数，请手动添加 */
    Bd21imuManager() = default;
    virtual ~Bd21imuManager() = default;

protected:
    virtual bool OnInitialize() override;
    virtual void Run() override;
    virtual void OnTerminate() override;
private:
    std::unique_ptr<mdc::Sample> samplePtr_;
};
} /* namespace bd21imu */
} /* namespace mdc */
#endif /* ARA_BD21IMU_BD21IMU_MANAGER_H */