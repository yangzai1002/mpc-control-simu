/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "fusionfollow_manager.h"
#include "mdc/fusionfollow_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::fusionfollow::FusionfollowManager fusionfollowManager;
        return fusionfollowManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Fusionfollow process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Fusionfollow process catch an unknown exception" << &std::endl;
    }
    return 0;
}