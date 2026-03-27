#include "adsfi_interface.h"



void AdsfiInterface::Init(){
    SetScheduleInfo("message");
	std::cout << "init" << std::endl;
	auto ptr = CustomStack::Instance();

    driver_ptr = std::make_shared<L106Driver>();

    // ptr->GetConfig("HuaweiL106:U-6/this","udp_buf_size", driver_ptr->driver_cfg.udp_buf_size);
    driver_ptr->driver_cfg.udp_buf_size = 655360;
    // ptr->GetConfig("HuaweiL106:U-6/this","lidar_type", driver_ptr->driver_cfg.lidar_type);
    ptr->GetConfig("HuaweiL106:U-6/this","sensor_id", driver_ptr->sensor_id_);
    ptr->GetConfig("HuaweiL106:U-6/this","native_ip", driver_ptr->driver_cfg.native_ip);
    ptr->GetConfig("HuaweiL106:U-6/this","phase_lock", driver_ptr->driver_cfg.phase_lock); // 6000 means 60 Degree
    ptr->GetConfig("HuaweiL106:U-6/this","ten_ms_filter_l", driver_ptr->driver_cfg.ten_ms_filter_l);
    ptr->GetConfig("HuaweiL106:U-6/this","ten_ms_filter_r", driver_ptr->driver_cfg.ten_ms_filter_r);
    ptr->GetConfig("HuaweiL106:U-6/this","interval_ms", driver_ptr->driver_cfg.interval_ms);
    int debug_mask = 0;
    ptr->GetConfig("HuaweiL106:U-6/this","debug", debug_mask);
    driver_ptr->lidar_cali_config.decoder_debug = (debug_mask & 4);
    driver_ptr->lidar_cali_config.error_debug = debug_mask;
    driver_ptr->driver_cfg.debug_ts = (debug_mask & 1);
    driver_ptr->driver_cfg.debug = (debug_mask & 2);
    driver_ptr->lidar_cali_config.print_data = (debug_mask & 16);

    std::cout << "driver_ptr->driver_cfg.udp_buf_size = " << driver_ptr->driver_cfg.udp_buf_size << "\n";
    std::cout << "sensor_id = " << driver_ptr->sensor_id_ << "\n";
    // std::cout << "driver_ptr->driver_cfg.lidar_type = " << driver_ptr->driver_cfg.lidar_type <<"\n";
    std::cout << "phase_lock(degree x 100): " << driver_ptr->driver_cfg.phase_lock << "\n";
    std::cout << "driver_ptr->driver_cfg.native_ip = " << driver_ptr->driver_cfg.native_ip << "\n";
    // std::cout << "driver_ptr->driver_cfg.pointcloud_port = " << driver_ptr->driver_cfg.pointcloud_port << "\n";
    // std::cout << "driver_ptr->driver_cfg.checkalive_port = " << driver_ptr->driver_cfg.checkalive_port << "\n";
    // std::cout << "driver_ptr->driver_cfg.pub_pointcloud = " << driver_ptr->driver_cfg.pub_pointcloud << "\n";
    std::cout << "driver_ptr->driver_cfg.ten_ms_filter_l = " << driver_ptr->driver_cfg.ten_ms_filter_l << "\n";
    std::cout << "driver_ptr->driver_cfg.ten_ms_filter_r = " << driver_ptr->driver_cfg.ten_ms_filter_r << "\n";
    std::cout << "debug_mask = " << debug_mask << "\n";
    std::cout << "driver_ptr->lidar_cali_config.print_data = " << driver_ptr->lidar_cali_config.print_data << "\n";
    std::cout << "driver_ptr->lidar_cali_config.decoder_debug = " << driver_ptr->lidar_cali_config.decoder_debug
              << "\n";
    std::cout << "driver_ptr->lidar_cali_config.error_debug = " << driver_ptr->lidar_cali_config.error_debug
              << "\n";
    std::cout << "driver_ptr->driver_cfg.interval_ms = " << driver_ptr->driver_cfg.interval_ms << "\n";
    std::cout << "driver_ptr->driver_cfg.debug_ts = " << driver_ptr->driver_cfg.debug_ts << "\n";
    std::cout << "driver_ptr->driver_cfg.debug = " << driver_ptr->driver_cfg.debug << "\n";
    // std::cout << "UDP_BUFFER_SIZE = " << UDP_BUFFER_SIZE << "\n";

    std::string lidar_name = "front";
    std::string calibration_key = "SensorCalibration";
    std::string vehicle_param_key = "VehicleParam";

    driver_ptr->msop_config.ip = "239.255.0.1";
    driver_ptr->msop_config.native_ip = "192.168.1.125";
    driver_ptr->msop_config.port = 2368;
    driver_ptr->msop_config.native_port = 11000;

    driver_ptr->difop_config.ip = "192.168.1.201";
    driver_ptr->difop_config.port = 58000;
    driver_ptr->difop_config.native_ip = "192.168.1.125";
    driver_ptr->difop_config.native_port = 12000;

    ptr->GetConfig("HuaweiL106:U-6/this","native_ip", driver_ptr->difop_config.native_ip);
    ptr->GetConfig("HuaweiL106:U-6/this","difop_port", driver_ptr->difop_config.native_port);
    ptr->GetConfig("HuaweiL106:U-6/this","native_ip", driver_ptr->msop_config.native_ip);
    ptr->GetConfig("HuaweiL106:U-6/this","msop_port", driver_ptr->msop_config.native_port);
    ptr->GetConfig("HuaweiL106:U-6/this","lidar_name", lidar_name);

    ptr->GetConfig("HuaweiL106:U-6/this","calibration_key", calibration_key);
    ptr->GetConfig("HuaweiL106:U-6/this","vehicle_param_key", vehicle_param_key);

    ReadLidarConfig(calibration_key, vehicle_param_key, lidar_name, driver_ptr->lidar_cali_config,
                    driver_ptr.get());

    driver_ptr->Init();
}



int AdsfiInterface::Process(const std::string name,std::shared_ptr<ara::adsfi::MsgLidarFrame> &msg){
	L106DataInteract *exchange_ptr = L106DataInteract::Instance();
    exchange_ptr->GetLidarData(*msg);

    return 0;
}



int AdsfiInterface::Process(const std::string name,std::shared_ptr<ara::adsfi::SensorLidarPacket> &msg){
	L106DataInteract *exchange_ptr = L106DataInteract::Instance();
    exchange_ptr->GetPacketsData(*msg);
    return 0;

}

void AdsfiInterface::ReadLidarConfig(const std::string calibration_key, const std::string &vehicle_param_key,
                                const std::string &lidar_name, avos::driver::DecoderConfig &config, L106Driver *driver) {
    auto ptr = CustomStack::Instance();
    AINFO << "node path:" << ptr->GetNodeConfigPath();

    std::vector<float> lidar_t;
    if (ptr->GetProjectConfigArray(lidar_name + "/lidar_t", lidar_t)) {
        config.tf_x = lidar_t[0];
        config.tf_y = lidar_t[1];
        config.tf_z = lidar_t[2];
        AINFO << "config.tf_x: " << config.tf_x;
        AINFO << "config.tf_y: " << config.tf_y;
        AINFO << "config.tf_z: " << config.tf_z;
        driver->ec_remove("1022002");
    } else {
        AERROR << "Failed to read lidar_t parameter of " << lidar_name;
        driver->ec_add("1022002", "Failed to read lidar_t parameter");
        // exit(1);
        printf("Failed to read lidar_t parameter of %s\n", lidar_name.c_str());
        config.tf_x = 1.86;
        config.tf_y = 0.1;
        config.tf_z = 1.32;
    }

    std::vector<float> lidar_R;
    if (ptr->GetProjectConfigArray(lidar_name + "/lidar_R", lidar_R)) {
        // config.tf_roll = DEG2ARC * lidar_R[0];
        // config.tf_pitch = DEG2ARC * lidar_R[1];
        // config.tf_yaw = DEG2ARC * lidar_R[2];
        config.tf_roll = lidar_R[0];
        config.tf_pitch = lidar_R[1];
        config.tf_yaw = lidar_R[2];
        AINFO << "config.tf_roll: " << config.tf_roll;
        AINFO << "config.tf_pitch: " << config.tf_pitch;
        AINFO << "config.tf_yaw: " << config.tf_yaw;
        driver->ec_remove("1022002");
    } else {
        AERROR << "Failed to read lidar_R parameter";
        driver->ec_add("1022002", "Failed to read lidar_R parameter");
        printf("Failed to read lidar_t parameter of %s\n", lidar_name.c_str());
        config.tf_roll = 0.176;
        config.tf_pitch = -0.988;
        config.tf_yaw = -1.656;
        // exit(1);
    }

    Eigen::Quaternionf rotation = Eigen::AngleAxisf(config.tf_yaw, Eigen::Vector3f::UnitZ()) *
                                  Eigen::AngleAxisf(config.tf_pitch, Eigen::Vector3f::UnitY()) *
                                  Eigen::AngleAxisf(config.tf_roll, Eigen::Vector3f::UnitX());
    // matrix_R_ = rotation.matrix();
    // vec_t_.col(0) << config.tf_x, config.tf_y, config.tf_z;

    config.Init();
}
