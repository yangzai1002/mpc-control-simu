/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwl106lidarleft_manager.h"
#include "mdc/hwl106lidarleft_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwl106lidarleft::Hwl106lidarleftManager hwl106lidarleftManager;
        return hwl106lidarleftManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwl106lidarleft process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwl106lidarleft process catch an unknown exception" << &std::endl;
    }
    return 0;
}