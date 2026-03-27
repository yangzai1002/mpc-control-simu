#include "adsfi_interface.h"
#include "HuaweiFR671V2.hpp"



void AdsfiInterface::Init() {
    driver_ptr = std::make_shared<FR671>();

    driver_ptr->driver_cfg.udp_buf_size = 655360;

    auto ptr = CustomStack::Instance();
    ptr->GetConfig("HuaweiFR671V2", "radar_name", driver_ptr->driver_cfg.radar_name);
    ptr->GetConfig("HuaweiFR671V2", "sensor_id", driver_ptr->sensor_id_);
    ptr->GetConfig("HuaweiFR671V2", "host_ip", driver_ptr->driver_cfg.native_ip);
    ptr->GetConfig("HuaweiFR671V2", "target_ip", driver_ptr->driver_cfg.target_ip);
    ptr->GetConfig("HuaweiFR671V2", "data_port", driver_ptr->driver_cfg.data_port);
    ptr->GetConfig("HuaweiFR671V2", "heart_port", driver_ptr->driver_cfg.heart_port);
    ptr->GetConfig("HuaweiFR671V2", "vehicle_port", driver_ptr->driver_cfg.vehicle_port);
    ptr->GetConfig("HuaweiFR671V2", "interval_ms", driver_ptr->driver_cfg.interval_ms);
    ptr->GetConfig("HuaweiFR671V2", "debug", driver_ptr->driver_cfg.debug);
    driver_ptr->radar_cali_config.debug = driver_ptr->driver_cfg.debug;

    std::cout << "driver_ptr->driver_cfg.udp_buf_size = " << driver_ptr->driver_cfg.udp_buf_size << "\n";
    std::cout << "driver_ptr->driver_cfg.radar_name = " << driver_ptr->driver_cfg.radar_name << "\n";
    std::cout << "driver_ptr->sensor_id = " << driver_ptr->sensor_id_ << "\n";
    std::cout << "driver_ptr->driver_cfg.host_ip = " << driver_ptr->driver_cfg.native_ip << "\n";
    std::cout << "driver_ptr->driver_cfg.target_ip = " << driver_ptr->driver_cfg.target_ip << "\n";
    std::cout << "driver_ptr->driver_cfg.data_port = " << driver_ptr->driver_cfg.data_port << "\n";
    std::cout << "driver_ptr->driver_cfg.heart_port = " << driver_ptr->driver_cfg.heart_port << "\n";
    std::cout << "driver_ptr->driver_cfg.vehicle_port = " << driver_ptr->driver_cfg.vehicle_port << "\n";
    std::cout << "driver_ptr->driver_cfg.interval_ms = " << driver_ptr->driver_cfg.interval_ms << "\n";
    std::cout << "driver_ptr->driver_cfg.debug = " << driver_ptr->driver_cfg.debug << "\n";

    std::string calibration_key = "SensorCalibration";
    std::string vehicle_param_key = "VehicleParam";
    ptr->GetConfig("HuaweiFR671V2", "calibration_key", calibration_key);
    ptr->GetConfig("HuaweiFR671V2", "vehicle_param_key", vehicle_param_key);
    driver_ptr->driver_cfg.config_name = calibration_key;
    driver_ptr->driver_cfg.vehicle_param = vehicle_param_key;

    ReadLidarConfig(calibration_key, vehicle_param_key, driver_ptr->driver_cfg.radar_name, driver_ptr->radar_cali_config, driver_ptr.get());

    driver_ptr->Init(driver_ptr->sensor_id_, driver_ptr->driver_cfg.interval_ms);
    SetScheduleInfo("message");
}



int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::SensorRadarPointCloud> &msg) {
    FR671DataInteract *exchange_ptr = FR671DataInteract::Instance();
    exchange_ptr->GetRadarData(*msg);

    return 0;
}



int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::SensorRadarPacket> &msg) {
    FR671DataInteract *exchange_ptr = FR671DataInteract::Instance();
    exchange_ptr->GetPacketsData(*msg);

    return 0;
}


void AdsfiInterface::Callback(const std::string &name,  const std::shared_ptr<ara::adsfi::VehicleInformation> &msg_ptr)
{
    FR671DataInteract *exchange_ptr = FR671DataInteract::Instance();
    exchange_ptr->SetShiftPosition(msg_ptr->vehicle_act_state.shift_position);
}

void AdsfiInterface::Callback(const std::string &name, const std::shared_ptr<ara::adsfi::MsgHafLocation> &msg_ptr)
{
    FR671DataInteract *exchange_ptr = FR671DataInteract::Instance();
    exchange_ptr->SetNotifyImu(*msg_ptr);
}


void AdsfiInterface::ReadLidarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
                                     const std::string &radar_name, avos::driver::DecoderConfig &config, FR671 *driver) {
    auto ptr = CustomStack::Instance();
    ApInfo("init") << "node path:" << ptr->GetNodeConfigPath();

    std::vector<float> lidar_t;
    if (ptr->GetProjectConfigArray(radar_name + "/radar_t", lidar_t)) {
        config.tf_x = lidar_t[0];
        config.tf_y = lidar_t[1];
        config.tf_z = lidar_t[2];
        ApInfo("init") << "config.tf_x: " << config.tf_x;
        ApInfo("init") << "config.tf_y: " << config.tf_y;
        ApInfo("init") << "config.tf_z: " << config.tf_z;
        driver->ec_remove("1072001");
    }
    else
    {
        // AERROR << "Failed to read radar_t parameter of " << radar_name;
        driver->ec_add("1072001", "Failed to read radar_t parameter of " + radar_name, "init");
        // exit(1);
        // printf("Failed to read radar_t parameter of %s\n", radar_name.c_str());
        config.tf_x = 1.86;
        config.tf_y = 0.1;
        config.tf_z = 1.32;
    }

    std::vector<float> lidar_R;
    if (ptr->GetProjectConfigArray(radar_name + "/radar_R", lidar_R)) {
        config.tf_roll = lidar_R[0];
        config.tf_pitch = lidar_R[1];
        config.tf_yaw = lidar_R[2];
        ApInfo("init") << "config.tf_roll: " << config.tf_roll;
        ApInfo("init") << "config.tf_pitch: " << config.tf_pitch;
        ApInfo("init") << "config.tf_yaw: " << config.tf_yaw;
        driver->ec_remove("1072001");
    }
    else
    {
        // AERROR << "Failed to read radar_R parameter of " << radar_name;
        driver->ec_add("1072001", "Failed to read radar_R parameter of "+radar_name, "init");
        // exit(1);
        // printf("Failed to read radar_R parameter of %s\n", radar_name.c_str());
        config.tf_roll  = 0.176;
        config.tf_pitch = -0.988;
        config.tf_yaw   = -1.656;
    }

    config.Init();

    if (ptr->GetProjectConfigValue("vehicle_interface/wheel_base_length", config.wheel_base_length)) {
        ApInfo("init") << "config.wheel_base_length: " << config.wheel_base_length;
        driver->ec_remove("1072001");
    }
    else
    {
        // AERROR << "Failed to read vehicle_interface/wheel_base_length parameter of " << radar_name;
        driver->ec_add("1072001", "Failed to read wheel_base_length parameter of "+radar_name, "init");
        // printf("Failed to read vehicle_interface/wheel_base_length parameter of %s\n", radar_name.c_str());
        config.wheel_base_length = 2.7;
        // exit(1);
    }

    if (ptr->GetProjectConfigValue("vehicle_interface/left_reduce_ratio", config.reduce_ratio)) {
        ApInfo("init") << "config.reduce_ratio: " << config.reduce_ratio;
        driver->ec_remove("1072001");
    }
    else
    {
        // AERROR << "Failed to read vehicle_interface/left_reduce_ratio parameter of " << radar_name;
        driver->ec_add("1072001", "Failed to read left_reduce_ratio parameter of "+radar_name, "init");
        // exit(1);
        // printf("Failed to read vehicle_interface/left_reduce_ratio parameter of %s\n", radar_name.c_str());
        config.reduce_ratio = 16;
    }
}
