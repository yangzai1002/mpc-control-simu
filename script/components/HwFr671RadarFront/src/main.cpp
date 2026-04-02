/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwfr671radarfront_manager.h"
#include "mdc/hwfr671radarfront_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwfr671radarfront::Hwfr671radarfrontManager hwfr671radarfrontManager;
        return hwfr671radarfrontManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwfr671radarfront process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwfr671radarfront process catch an unknown exception" << &std::endl;
    }
    return 0;
}