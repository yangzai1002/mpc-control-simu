#include "adsfi_interface.h"

#define ERRORCODE_CONFIG_READ_ERROR "1052001" // 配置参数读取失败

AdsfiInterface::AdsfiInterface() {
}

AdsfiInterface::~AdsfiInterface() {
}

void AdsfiInterface::Init() {
    SetScheduleInfo("timmer", 20);

    auto ptr = CustomStack::Instance();

    std::string addr;
    int32_t     port;
    std::string topic_name;
    int32_t     timeout_local;
    std::string prefix;

    app_common::StringUtils::trim(prefix);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += "/";
    }
    ApInfo("vehicle_chassis_recv") << "config_path_prefix: " << prefix;

    std::string base = prefix + "auto/canbus/receive/";

    auto full_path = base + "addr";
    if (!ptr->GetProjectConfigValue(full_path, addr) || addr.empty()) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        ApInfo("vehicle_chassis_recv") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    full_path = base + "port";
    if (!ptr->GetProjectConfigValue(base + "port", port) || port <= 0) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        ApInfo("vehicle_chassis_recv") << "初始化参数加载错误: read project config " << base + "port"
                                       << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    full_path = base + "topic_name";
    if (!ptr->GetProjectConfigValue(full_path, topic_name) || topic_name.empty()) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        ApInfo("vehicle_chassis_recv") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    full_path = base + "over_time";
    if (!ptr->GetProjectConfigValue(full_path, timeout_local) || timeout_local <= 0) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        ApInfo("vehicle_chassis_recv") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    // std::string endpoint = "tcp://" + ip + ":" + std::to_string(port);
    std::string endpoint = addr;
    ApInfo("vehicle_chassis_recv") << " ip: ==== " << addr;
    ApInfo("vehicle_chassis_recv") << " port: ==== " << port;
    ApInfo("vehicle_chassis_recv") << " topic_name: ==== " << topic_name;
    ApInfo("vehicle_chassis_recv") << " timeout==== " << timeout_local;
    ApInfo("vehicle_chassis_recv") << "连接地址: ====" << endpoint << " 话题: " << topic_name;

    try {
        XMidTrackReceiveByZMQDataInteract::Instance()->Init(endpoint, topic_name, timeout_local);
        ApInfo("vehicle_chassis_recv") << "ZMQ接收初始化成功-----";
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.remove("1062001");
    } catch (const std::exception &e) {
        ApInfo("vehicle_chassis_recv") << "初始化失败-----:" << e.what();
    }
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    XMidTrackReceiveByZMQDataInteract::Instance()->GetVehicleInformation(*msg);
    return 0;
}
