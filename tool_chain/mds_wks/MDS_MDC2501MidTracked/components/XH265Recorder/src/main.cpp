/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "xh265recorder_manager.h"
#include "mdc/xh265recorder_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::xh265recorder::Xh265recorderManager xh265recorderManager;
        return xh265recorderManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Xh265recorder process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Xh265recorder process catch an unknown exception" << &std::endl;
    }
    return 0;
}