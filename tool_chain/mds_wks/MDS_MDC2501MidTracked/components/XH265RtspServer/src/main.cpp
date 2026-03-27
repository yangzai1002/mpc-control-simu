/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "xh265rtspserver_manager.h"
#include "mdc/xh265rtspserver_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::xh265rtspserver::Xh265rtspserverManager xh265rtspserverManager;
        return xh265rtspserverManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Xh265rtspserver process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Xh265rtspserver process catch an unknown exception" << &std::endl;
    }
    return 0;
}