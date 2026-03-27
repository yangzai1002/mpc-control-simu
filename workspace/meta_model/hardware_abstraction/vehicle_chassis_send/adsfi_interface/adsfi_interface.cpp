#include "adsfi_interface.h"

void AdsfiInterface::Init() {
    auto        ptr = CustomStack::Instance();
    std::string addr;
    int32_t     port;
    std::string topic_name;
    int32_t     timeout_local = 200;
    std::string prefix;

    app_common::StringUtils::trim(prefix);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += "/";
    }
    ApInfo("vehicle_chassis_send") << "config_path_prefix: " << prefix;
    std::string base = prefix + "auto/canbus/send/";

    auto full_path = base + "addr";
    if (!ptr->GetProjectConfigValue(full_path, addr) || addr.empty()) {
        XMidTrackSendByZMQDataInteract::Instance()->_fault.add(_ERRORCODE_CONFIG_READ, full_path);
        ApInfo("vehicle_chassis_send") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ;
        return;
    }

    full_path = base + "port";
    if (!ptr->GetProjectConfigValue(base + "port", port) || port <= 0) {
        XMidTrackSendByZMQDataInteract::Instance()->_fault.add(_ERRORCODE_CONFIG_READ, full_path);
        ApInfo("vehicle_chassis_send") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ;
        return;
    }

    full_path = base + "topic_name";
    if (!ptr->GetProjectConfigValue(full_path, topic_name) || topic_name.empty()) {
        XMidTrackSendByZMQDataInteract::Instance()->_fault.add(_ERRORCODE_CONFIG_READ, full_path);
        ApInfo("vehicle_chassis_send") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ;
        return;
    }

    full_path = base + "over_time";
    if (!ptr->GetProjectConfigValue(full_path, timeout_local) || timeout_local <= 0) {
        XMidTrackSendByZMQDataInteract::Instance()->_fault.add(_ERRORCODE_CONFIG_READ, full_path);
        ApInfo("vehicle_chassis_send") << "初始化参数加载错误: read project config " << full_path
                                       << " failed, 上报故障码: " << _ERRORCODE_CONFIG_READ;
        return;
    }

    timeout_ms = timeout_local;

    std::string endpoint = addr;

    ApInfo("vehicle_chassis_send") << " addr: ==== " << addr;
    ApInfo("vehicle_chassis_send") << " port: ==== " << port;
    ApInfo("vehicle_chassis_send") << " topic_name: ==== " << topic_name;
    ApInfo("vehicle_chassis_send") << " time_out(ms)==== " << timeout_ms;
    ApInfo("vehicle_chassis_send") << "连接地址ok111: ====" << endpoint << " 话题: " << topic_name;

    try {
        XMidTrackSendByZMQDataInteract::Instance()->Init(endpoint, topic_name, timeout_ms);

        ApInfo("vehicle_chassis_send") << "ZMQ发送初始化成功-----";
        std::cout << "yrf----ZMQ发送初始化成功-----" << std::endl;
        XMidTrackSendByZMQDataInteract::Instance()->_fault.remove(_ERRORCODE_CONFIG_READ);
    } catch (const std::exception &e) {
        ApInfo("vehicle_chassis_send") << "初始化失败-----:" << e.what();
        XMidTrackSendByZMQDataInteract::Instance()->_fault.add(_ERRORCODE_CONFIG_READ,
                                                               std::string("init_exception:") + e.what());
    }

    update_time.store(-1.0);

    std::thread([&]() {
        while (true) {
            if (update_time.load() < 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            double time_diff = avos::common::NodeTime::Now().ToSecond() - update_time.load();
            if (time_diff > 0.1) {
                ara::adsfi::VehicleActControl data;
                GetInnerActMsg(data);
                XMidTrackSendByZMQDataInteract::Instance()->updateSendMsg(data, sensor_ctl, fault_data);
                ApInfo("vehicle_chassis_send") << "update old data";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::VehicleActControl> &msg) {
    SetInnerActMsg(msg);
    ApInfo("vehicle_chassis_send") << "CALLBACK:: " << msg->lon_control.target_speed;
    XMidTrackSendByZMQDataInteract::Instance()->updateSendMsg(*msg, sensor_ctl, fault_data);
}
