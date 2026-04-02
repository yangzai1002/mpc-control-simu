/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwcameralist_manager.h"
#include "mdc/hwcameralist_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwcameralist::HwcameralistManager hwcameralistManager;
        return hwcameralistManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwcameralist process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwcameralist process catch an unknown exception" << &std::endl;
    }
    return 0;
}