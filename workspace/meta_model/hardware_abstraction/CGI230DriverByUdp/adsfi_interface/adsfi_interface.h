
#ifndef __ADSFI_INTERFACE_H__
#define __ADSFI_INTERFACE_H__

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "XCGI230DriverByUdp.hpp"
#include "adsfi_manager/base_adsfi_interface.h"
#include "adsfi_manager/lock_queue.h"
#include "ara/adsfi/impl_type_msghafgnssinfo.h"
#include "ara/adsfi/impl_type_msghafimu.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_msghafwheelspeedlist.h"
#include "config_loader/custom_stack.h"
#include "geometry_tools.hpp"
#include "log/log.h"
#include "monitor/faulthandle_api.hpp"

class AdsfiInterface : public BaseAdsfiInterface {
public:
    AdsfiInterface();

    ~AdsfiInterface();

    void Init() override;

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafIMU> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &msg);

    int Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &msg);

private:
    std::shared_ptr<CGI230DriverByUdp::CGI230UdpRecvData> _driver_ptr;

public:
    BASE_TEMPLATE_FUNCION
};
#endif
