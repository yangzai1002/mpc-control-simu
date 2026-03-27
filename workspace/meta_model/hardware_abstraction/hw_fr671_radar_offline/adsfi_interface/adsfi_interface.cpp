#include "adsfi_interface.h"
#include "HuaweiFR671Decode.hpp"



void AdsfiInterface::Init() {
    auto ptr = CustomStack::Instance();

    driver_ptr = std::make_shared<FR671DriverOffline>();

    if(ptr->GetConfig("HuaweiFR671V2" ,"radar_name", driver_ptr->radar_name_) != 0)
    {
        printf("read radar_name failed\n");
    }
    if(ptr->GetConfig("HuaweiFR671V2" ,"sensor_id", driver_ptr->sensor_id_) != 0)
    {
        printf("read sensor_id failed\n");
    }
    if(ptr->GetConfig("HuaweiFR671V2" ,"interval_ms", driver_ptr->driver_cfg.interval_ms) != 0)
    {
        printf("read interval_ms failed\n");
    }
    if(ptr->GetConfig("HuaweiFR671V2" ,"debug", driver_ptr->driver_cfg.debug) != 0)
    {
        printf("read debug failed\n");
    }
    if(ptr->GetConfig("HuaweiFR671V2" ,"filter_obj", driver_ptr->radar_cali_config.filter_obj) != 0)
    {
        printf("read filter_obj failed\n");
    }
    driver_ptr->radar_cali_config.debug = driver_ptr->driver_cfg.debug;
    driver_ptr->driver_cfg.radar_name = driver_ptr->radar_name_;

    std::cout << "driver_ptr->radar_name = " << driver_ptr->radar_name_ <<"\n";
    std::cout << "driver_ptr->sensor_id = " << driver_ptr->sensor_id_ <<"\n";
    std::cout << "driver_ptr->driver_cfg.interval_ms = " << driver_ptr->driver_cfg.interval_ms << "\n";
    std::cout << "driver_ptr->driver_cfg.debug = " << driver_ptr->driver_cfg.debug << "\n";
    std::cout << "driver_ptr->radar_cali_config.filter_obj = " << driver_ptr->radar_cali_config.filter_obj << "\n";

    std::string calibration_key = "SensorCalibration";
    std::string vehicle_param_key = "VehicleParam";
    ptr->GetConfig("HuaweiFR671V2" ,"calibration_key", calibration_key);
    ptr->GetConfig("HuaweiFR671V2" ,"vehicle_param_key", vehicle_param_key);
    driver_ptr->driver_cfg.config_name = calibration_key;
    driver_ptr->driver_cfg.vehicle_param = vehicle_param_key;

    ReadRadarConfig(calibration_key, vehicle_param_key, driver_ptr->driver_cfg.radar_name, driver_ptr->radar_cali_config);
    driver_ptr->decoder.Init(driver_ptr->radar_cali_config, driver_ptr->sensor_id_, driver_ptr->driver_cfg.interval_ms);

    SetScheduleInfo("message");
}



int AdsfiInterface::Process(const std::string &name,std::shared_ptr<ara::adsfi::SensorRadarPointCloud> &msg) {

    ara::adsfi::SensorRadarPacket packets;
    GetRadarPackets(packets);
    driver_ptr->DecodePackets(packets, *msg);

    return 0;
}



void AdsfiInterface::Callback(const std::string &name,const std::shared_ptr<ara::adsfi::SensorRadarPacket> &msg) {

    SetRadarPackets(msg);
}



void AdsfiInterface::ReadRadarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
                            const std::string &radar_name, avos::driver::DecoderConfig &config) {
    auto ptr = CustomStack::Instance();
    AINFO << "node path:" << ptr->GetNodeConfigPath();
    AINFO << "radar_name:" << radar_name;

    std::vector<float> lidar_t;
    if (ptr->GetProjectConfigArray(radar_name+"/radar_t", lidar_t)) {
        config.tf_x = lidar_t[0];
        config.tf_y = lidar_t[1];
        config.tf_z = lidar_t[2];
        AINFO << "config.tf_x: " << config.tf_x;
        AINFO << "config.tf_y: " << config.tf_y;
        AINFO << "config.tf_z: " << config.tf_z;
    }
    else
    {
        AERROR << "Failed to read radar_t parameter of " << radar_name;
        // exit(1);
        printf("Failed to read radar_t parameter of %s\n", radar_name.c_str());
        config.tf_x = 1.86;
        config.tf_y = 0.1;
        config.tf_z = 1.32;
    }

    std::vector<float> lidar_R;
    if (ptr->GetProjectConfigArray(radar_name+"/radar_R", lidar_R)) {
        config.tf_roll = lidar_R[0];
        config.tf_pitch = lidar_R[1];
        config.tf_yaw = lidar_R[2];
        AINFO << "config.tf_roll: " << config.tf_roll;
        AINFO << "config.tf_pitch: " << config.tf_pitch;
        AINFO << "config.tf_yaw: " << config.tf_yaw;
    }
    else
    {
        AERROR << "Failed to read radar_R parameter of " << radar_name;
        // exit(1);
        printf("Failed to read radar_R parameter of %s\n", radar_name.c_str());
        config.tf_roll  = 0.176;
        config.tf_pitch = -0.988;
        config.tf_yaw   = -1.656;
    }

    config.Init();
    RadarShmParam::SetRadarModuleParam(radar_name, config.pre_rot_axis_0,
                                    config.pre_rot_axis_1,
                                    config.pre_rot_axis_2,
                                    config.pre_rot_degree_0,
                                    config.pre_rot_degree_1,
                                    config.pre_rot_degree_2,
                                    config.tf_x,
                                    config.tf_y,
                                    config.tf_z,
                                    config.tf_roll,
                                    config.tf_pitch,
                                    config.tf_yaw
                                   );

}