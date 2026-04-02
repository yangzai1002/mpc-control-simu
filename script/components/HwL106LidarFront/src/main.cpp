/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwl106lidarfront_manager.h"
#include "mdc/hwl106lidarfront_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwl106lidarfront::Hwl106lidarfrontManager hwl106lidarfrontManager;
        return hwl106lidarfrontManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwl106lidarfront process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwl106lidarfront process catch an unknown exception" << &std::endl;
    }
    return 0;
}