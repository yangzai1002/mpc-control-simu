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

    if (ptr->GetConfig("XMidTrackReceiveByZMQ:U-2/this", "config_path_prefix", prefix) < 0) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, "config_path_prefix");
        AINFO << "初始化参数加载错误: read local config config_path_prefix failed, 上报故障码: "
              << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    app_common::StringUtils::trim(prefix);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += "/";
    }
    AINFO << "config_path_prefix: " << prefix;

    std::string base = prefix + "canbus/receive/";

    auto full_path = base + "addr";
    if (!ptr->GetProjectConfigValue(full_path, addr) || addr.empty()) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        AINFO << "初始化参数加载错误: read project config " << full_path
              << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    full_path = base + "port";
    if (!ptr->GetProjectConfigValue(base + "port", port) || port <= 0) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        AINFO << "初始化参数加载错误: read project config " << base + "port"
              << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    full_path = base + "topic_name";
    if (!ptr->GetProjectConfigValue(full_path, topic_name) || topic_name.empty()) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        AINFO << "初始化参数加载错误: read project config " << full_path
              << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    full_path = base + "over_time";
    if (!ptr->GetProjectConfigValue(full_path, timeout_local) || timeout_local <= 0) {
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.add(ERRORCODE_CONFIG_READ_ERROR, full_path);
        AINFO << "初始化参数加载错误: read project config " << full_path
              << " failed, 上报故障码: " << ERRORCODE_CONFIG_READ_ERROR;
        return;
    }

    // std::string endpoint = "tcp://" + ip + ":" + std::to_string(port);
    std::string endpoint = addr;
    AINFO << " ip: ==== " << addr;
    AINFO << " port: ==== " << port;
    AINFO << " topic_name: ==== " << topic_name;
    AINFO << " timeout==== " << timeout_local;
    AINFO << "连接地址: ====" << endpoint << " 话题: " << topic_name;

    try {
        XMidTrackReceiveByZMQDataInteract::Instance()->Init(endpoint, topic_name, timeout_local);
        AINFO << "ZMQ接收初始化成功-----";
        XMidTrackReceiveByZMQDataInteract::Instance()->_fault.remove("1062001");
    } catch (const std::exception &e) {
        AINFO << "初始化失败-----:" << e.what();
    }
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::VehicleInformation> &msg) {
    XMidTrackReceiveByZMQDataInteract::Instance()->GetVehicleInformation(*msg);
    return 0;
}
