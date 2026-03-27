/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "hwfr671radarleft_manager.h"
#include "mdc/hwfr671radarleft_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::hwfr671radarleft::Hwfr671radarleftManager hwfr671radarleftManager;
        return hwfr671radarleftManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Hwfr671radarleft process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Hwfr671radarleft process catch an unknown exception" << &std::endl;
    }
    return 0;
}