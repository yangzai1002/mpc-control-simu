#include "adsfi_interface.h"

AdsfiInterface::AdsfiInterface() {
    std::int32_t port;
    auto         ptr = CustomStack::Instance();
    ptr->GetConfig("CGI230DriverByUdp", "port", port);
    _driver_ptr = std::make_shared<CGI230DriverByUdp::CGI230UdpRecvData>(port);
    _driver_ptr->start();
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 8); // 为什么是 8ms 呢？防止底层队列堆积，造成定位延时
    avos::common::AvosNode::Init("CGI230DriverByUdp", true, "/opt/usr/zxz/log/", "/opt/usr/zxz/log/",
                                 "/opt/usr/zxz/log/");
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg) {
    return _driver_ptr->GetSensorGps(msg);
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafIMU> &msg) {
    return -1;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &msg) {
    AINFO << "here";
    return _driver_ptr->GetLocationInfo(msg);
    // return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &msg) {
    return -1;
}
