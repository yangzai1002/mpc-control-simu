/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "xzmqrc_manager.h"
#include "mdc/xzmqrc_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::xzmqrc::XzmqrcManager xzmqrcManager;
        return xzmqrcManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Xzmqrc process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Xzmqrc process catch an unknown exception" << &std::endl;
    }
    return 0;
}