/*
 * Copyright (c) Shenzhen Yinwang Intelligent Technologies Co., Ltd. 2024. All rights reserved.
 * Description: main代码模板
 */
#include <iostream>
#include "vehiclechassisrecv_manager.h"
#include "mdc/vehiclechassisrecv_logger.h"

int32_t main(int32_t argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        mdc::vehiclechassisrecv::VehiclechassisrecvManager vehiclechassisrecvManager;
        return vehiclechassisrecvManager.Execute();
    } catch (const std::exception &exc) {
        std::cerr << "Vehiclechassisrecv process catch an exception: " << exc.what() << &std::endl;
    } catch (...) {
        std::cerr << "Vehiclechassisrecv process catch an unknown exception" << &std::endl;
    }
    return 0;
}