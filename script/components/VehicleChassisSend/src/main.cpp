/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "vehiclechassissend_manager.h"
#include "mdc/vehiclechassissend_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::vehiclechassissend::VehiclechassissendManager vehiclechassissendManager;
        return vehiclechassissendManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Vehiclechassissend process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Vehiclechassissend process catch an unknown exception" << &std::endl;
    }
    return 0;
}