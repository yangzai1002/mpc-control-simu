/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "xh265encoder_manager.h"
#include "mdc/xh265encoder_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::xh265encoder::Xh265encoderManager xh265encoderManager;
        return xh265encoderManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Xh265encoder process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Xh265encoder process catch an unknown exception" << &std::endl;
    }
    return 0;
}