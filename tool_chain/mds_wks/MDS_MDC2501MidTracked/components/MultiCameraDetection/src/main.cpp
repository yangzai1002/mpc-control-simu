/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "multicameradetection_manager.h"
#include "mdc/multicameradetection_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::multicameradetection::MulticameradetectionManager multicameradetectionManager;
        return multicameradetectionManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Multicameradetection process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Multicameradetection process catch an unknown exception" << &std::endl;
    }
    return 0;
}