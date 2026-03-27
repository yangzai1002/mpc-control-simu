/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwl106lidarright_manager.h"
#include "mdc/hwl106lidarright_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwl106lidarright::Hwl106lidarrightManager hwl106lidarrightManager;
        return hwl106lidarrightManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwl106lidarright process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwl106lidarright process catch an unknown exception" << &std::endl;
    }
    return 0;
}