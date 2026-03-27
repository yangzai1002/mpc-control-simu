/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwfr671radarright_manager.h"
#include "mdc/hwfr671radarright_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwfr671radarright::Hwfr671radarrightManager hwfr671radarrightManager;
        return hwfr671radarrightManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwfr671radarright process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwfr671radarright process catch an unknown exception" << &std::endl;
    }
    return 0;
}