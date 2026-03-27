/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "xh265rtppusher_manager.h"
#include "mdc/xh265rtppusher_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::xh265rtppusher::Xh265rtppusherManager xh265rtppusherManager;
        return xh265rtppusherManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Xh265rtppusher process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Xh265rtppusher process catch an unknown exception" << &std::endl;
    }
    return 0;
}