/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "location_manager.h"
#include "mdc/location_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::location::LocationManager locationManager;
        return locationManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Location process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Location process catch an unknown exception" << &std::endl;
    }
    return 0;
}