/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "control_manager.h"
#include "mdc/control_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::control::ControlManager controlManager;
        return controlManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Control process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Control process catch an unknown exception" << &std::endl;
    }
    return 0;
}