/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "lidardetection_manager.h"
#include "mdc/lidardetection_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::lidardetection::LidardetectionManager lidardetectionManager;
        return lidardetectionManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Lidardetection process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Lidardetection process catch an unknown exception" << &std::endl;
    }
    return 0;
}