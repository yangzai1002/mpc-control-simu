/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "sensorvisualize_manager.h"
#include "mdc/sensorvisualize_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::sensorvisualize::SensorvisualizeManager sensorvisualizeManager;
        return sensorvisualizeManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Sensorvisualize process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Sensorvisualize process catch an unknown exception" << &std::endl;
    }
    return 0;
}