/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "bd21imu_manager.h"
#include "mdc/bd21imu_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::bd21imu::Bd21imuManager bd21imuManager;
        return bd21imuManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Bd21imu process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Bd21imu process catch an unknown exception" << &std::endl;
    }
    return 0;
}