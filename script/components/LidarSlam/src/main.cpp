/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "lidarslam_manager.h"
#include "mdc/lidarslam_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::lidarslam::LidarslamManager lidarslamManager;
        return lidarslamManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Lidarslam process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Lidarslam process catch an unknown exception" << &std::endl;
    }
    return 0;
}