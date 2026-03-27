#include "LocationInfoHandler.hpp"
#include "adsfi_interface.h"

void AdsfiInterface::Init() {
    driver_ptr = std::make_shared<BD21Driver>();
    loc_fusion_data_ptr = std::make_shared<avos::BD21::LocFusionData>();
    sensor_imu_data_ptr = std::make_shared<avos::BD21::SensorImuData>();
    sensor_imu_sync_data_ptr = std::make_shared<avos::BD21::SensorImuData>();
    gnss_data_ptr = std::make_shared<avos::BD21::GnssData>();
    wheel_speed_data_ptr = std::make_shared<avos::BD21::WheelSpeedData>();

    auto ptr = CustomStack::Instance();

    ptr->GetConfig("BD21ByCan:U-0/this", "can_device", driver_ptr->can_config_.can_name);
    ptr->GetConfig("BD21ByCan:U-0/this", "can_device_ws", driver_ptr->can_config_ws_.can_name);
    ptr->GetConfig("BD21ByCan:U-0/this", "use_sys_time", driver_ptr->use_sys_time_);
    ptr->GetConfig("BD21ByCan:U-0/this", "record_data", driver_ptr->record_data_);
    ptr->GetConfig("BD21ByCan:U-0/this", "debug", driver_ptr->debug_);
    ptr->GetConfig("BD21ByCan:U-0/this", "use_raw_data", driver_ptr->use_raw_data_);
    driver_ptr->Init();
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafLocation> &msg) {
    auto change_ptr = BD21DataInteract::Instance();
    change_ptr->GetLocFusion(*loc_fusion_data_ptr);
    change_ptr->GetImuData(*sensor_imu_sync_data_ptr, true);

    msg->header.seq = loc_fusion_seq++;
    double t = NavboxUtc2Unix(loc_fusion_data_ptr->week, loc_fusion_data_ptr->utc_time);
    msg->header.timestamp.sec = (unsigned int)t;
    msg->header.timestamp.nsec = (unsigned int)((t - msg->header.timestamp.sec) * 1e9);
    msg->header.frameID = "/base_link";

    //MsgPoint3D	经纬高
    // msg->locationState = loc_fusion_data_ptr->status;
    msg->llh.x = loc_fusion_data_ptr->lon;
    msg->llh.y = loc_fusion_data_ptr->lat;
    msg->llh.z = loc_fusion_data_ptr->height;

    //MsgHafPoseWithCovariance	位姿信息
    msg->pose.pose.orientation.x = loc_fusion_data_ptr->roll;
    msg->pose.pose.orientation.y = loc_fusion_data_ptr->pitch;
    msg->pose.pose.orientation.z = GeometryTool::Heading2Angle(loc_fusion_data_ptr->yaw);

    GeoPointGPS gps;
    gps.lon = msg->llh.x;
    gps.lat = msg->llh.y;
    gps.heading = msg->pose.pose.orientation.z;

    GeoPointGCCS gccs;
    GeometryTool::GPS2GCCS(gps, gccs);

    msg->pose.pose.position.x = gccs.xg;
    msg->pose.pose.position.y = gccs.yg;GeoPointGPS  temp_point_gps(msg->longitude, msg->latitude, msg->attitude.z);
    GeoPointGCCS temp_point_gccs;
    bool         trans_flag = true;
    if (std::isnan(msg->longitude) == false && std::isnan(msg->latitude) == false &&
        std::isnan(msg->attitude.z) == false) {
        GeometryTool::GPS2GCCS(temp_point_gps, temp_point_gccs);
        //MsgPoint3D UTM坐标位置
        msg->utmPosition.x = temp_point_gccs.xg;
        msg->utmPosition.y = temp_point_gccs.yg;
        // msg->utmPosition.z = ;
    }

    msg->pose.pose.position.z = loc_fusion_data_ptr->height;

    //MsgHafTwistWithCovariance	速度信息
    msg->velocity.twist.linear.x = loc_fusion_data_ptr->east_speed;
    msg->velocity.twist.linear.y = loc_fusion_data_ptr->north_speed;
    msg->velocity.twist.linear.z = loc_fusion_data_ptr->up_speed;

    msg->velocity.twist.angular.x = sensor_imu_sync_data_ptr->gyro_x;
    msg->velocity.twist.angular.y = sensor_imu_sync_data_ptr->gyro_y;
    msg->velocity.twist.angular.z = sensor_imu_sync_data_ptr->gyro_z;

    //MsgHafAccelWithCovariance	加速度信息
    msg->acceleration.accel.linear.x = sensor_imu_sync_data_ptr->acce_x;
    msg->acceleration.accel.linear.y = sensor_imu_sync_data_ptr->acce_y;
    msg->acceleration.accel.linear.z = sensor_imu_sync_data_ptr->acce_z;

    msg->v = loc_fusion_data_ptr->v;

    //unsigned short 定位状态 0:无定位; 1:单点定位定 2:伪距差分 4:固定解 5:浮点解 6:SBAS 定位 7: ppp定位(html 定义)
    //惯导状态(设备定义):
    //0-惯导正在启动;1-惯导启动完成;2-正在对准，卫星辅助;3-正在对准，装订模式;4-导航状态，惯导/卫星/轮速组合模式:5-导航状态，惯导/卫星组合模式;6-导航状态，惯导/轮速组合模式;7-导航状态，纯惯导模式
    msg->locationState = loc_fusion_data_ptr->status;

    //unsigned short gnss状态 0:无定位; 1:单点定位定 2:伪距差分 4:固定解 5:浮点解 6:SBAS 定位 7: ppp定位(html 定义)
    // 位置初始化标志位(设备定义)，0-定位数据无效;1-单点定位;2-伪距差分定位;3-双频定位;4-RTK定位，四定解:5-RTK定位，浮点解:6-SBAS定位:7-PPP
    msg->gnssState = loc_fusion_data_ptr->gps_status;

    //融合标志：0-VI, inertial, camera；1-RI, inertial, radar；2-LI, inertial, lidar；3-withLIC,inertial, lidar, camera
    // msg->odomType = ;
    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafGnssInfo> &msg) {
    auto change_ptr = BD21DataInteract::Instance();
    change_ptr->GetGpsData(*gnss_data_ptr);

     //double 纬度信息[-180,180]
    msg->latitude = gnss_data_ptr->lat;

    //double 经度信息[-180,180]
    msg->longitude = gnss_data_ptr->lon;

    //double 海拔信息(m)
    msg->elevation = gnss_data_ptr->height;

    //int UTM区号
    // msg->utmZoneNum = ;

    //unsigned char UTM区号
    // msg->utmZoneChar = ;

    //unsigned char 航向数据有效性 0：无效 1：有效
    // msg->heading_valid = ;

    //unsigned char 位置数据有效性 0：无效 1：有效
    // msg->pos_valid = ;

    //MsgPoint3D 三轴姿态 x->roll y->pitch z->yaw
    // msg->attitude.x = ;
    // msg->attitude.y = ;
    msg->attitude.z = gnss_data_ptr->heading;

    GeoPointGPS  temp_point_gps(msg->longitude, msg->latitude, msg->attitude.z);
    GeoPointGCCS temp_point_gccs;
    bool         trans_flag = true;
    if (std::isnan(msg->longitude) == false && std::isnan(msg->latitude) == false &&
        std::isnan(msg->attitude.z) == false) {
        GeometryTool::GPS2GCCS(temp_point_gps, temp_point_gccs);
        //MsgPoint3D UTM坐标位置
        msg->utmPosition.x = temp_point_gccs.xg;
        msg->utmPosition.y = temp_point_gccs.yg;
        // msg->utmPosition.z = ;
    }

    //MsgPoint3D 位置标准差
    // msg->sdPosition.x = ;
    // msg->sdPosition.y = ;
    // msg->sdPosition.z = ;

    //MsgPoint3D 速度标准差
    // msg->sdVelocity.x = ;
    // msg->sdVelocity.y = ;
    // msg->sdVelocity.z = ;

    //MsgPoint3D 姿态标准差
    // msg->sdAttitude.x = ;
    // msg->sdAttitude.y = ;
    // msg->sdAttitude.z = ;

    //double 时间
    msg->second = NavboxUtc2Unix(gnss_data_ptr->week, gnss_data_ptr->utc_time);

    //int 使用卫星数
    msg->satUseNum = gnss_data_ptr->satenum;

    //int 可见卫星数
    // msg->satInViewNum = ;

    //unsigned short 求解状态
    msg->solutionStatusDual = gnss_data_ptr->status;

    //unsigned short 定位状态 GPS状态位 0:无定位; 1:单点定位定 2:伪距差分 4:固定解 5:浮点解 6:SBAS 定位 7: ppp定位
    msg->positionType = gnss_data_ptr->status;

    //MsgPoint3D 线速度信息
    msg->linearVelocity.x = gnss_data_ptr->vertical_spd;
    // msg->linearVelocity.y = ;
    msg->linearVelocity.z = gnss_data_ptr->horizontal_spd;

    //MsgPoint3D 双天线姿态
    // msg->attitudeDual.x = ;
    // msg->attitudeDual.y = ;
    msg->attitudeDual.z = gnss_data_ptr->track_angle;

    //MsgPoint3D 双天线角度标准差
    // msg->sdAngleDual.x = ;
    // msg->sdAngleDual.y = ;
    // msg->sdAngleDual.z = ;

    //double 双天线距离
    msg->baseLineLengthDual = gnss_data_ptr->base_length;

    //int 双天线求解状态
    // msg->solutionStatusDual = ;

    //int 双天线定位状态
    msg->positionTypeDual = gnss_data_ptr->diff_age;

    //int 双天线求解源
    // msg->solutionSourceDual = ;

    //double cep68数据位
    // msg->cep68 = ;

    //double cep95数据位
    // msg->cep95 = ;

    //float pDop数据位
    // msg->pDop = ;

    //float hDop数据位
    // msg->hDop = ;

    //float vDop数据位
    // msg->vDop = ;

    msg->header.timestamp.sec = ADSFI_GET_SEC(msg->second);
    msg->header.timestamp.nsec = ADSFI_GET_NSEC(msg->second);

    msg->header.seq = gps_seq++;
    msg->header.frameID = "/base_link";

    return 0;
}

int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafWheelSpeedList> &msg) {
    auto change_ptr = BD21DataInteract::Instance();
    change_ptr->GetWheelSpeedData(*wheel_speed_data_ptr);

    ara::adsfi::MsgHafWheelSpeed left_rear_ws;
    ara::adsfi::MsgHafWheelSpeed right_rear_ws;
    ara::adsfi::MsgHafWheelSpeed left_front_ws;
    ara::adsfi::MsgHafWheelSpeed right_front_ws;
    double                       t = NavboxUtc2Unix(wheel_speed_data_ptr->week, wheel_speed_data_ptr->utc_time);
    msg->header.seq = wheel_speed_seq++;
    msg->header.frameID = "/base_link";
    msg->header.timestamp.sec = ADSFI_GET_SEC(t);
    msg->header.timestamp.nsec = ADSFI_GET_NSEC(t);

    //车轮运动方向，取值：0：向前，1：向后，2：静止，3：无效
    // left_rear_ws.wheelDirection = 0;
    // right_rear_ws.wheelDirection = 0;
    // left_front_ws.wheelDirection = 0;
    // right_front_ws.wheelDirection = 0;

    //轮速脉冲，单位：计数值，逻辑：车轮向前运动为正，车轮向后运动为负
    // left_rear_ws.wheelCount = 0;
    // right_rear_ws.wheelCount = 0;
    // left_front_ws.wheelCount = 0;
    // right_front_ws.wheelCount = 0;

    left_rear_ws.wheelSpeedMpsValid = 1;
    right_rear_ws.wheelSpeedMpsValid = 1;
    left_front_ws.wheelSpeedMpsValid = 1;
    right_front_ws.wheelSpeedMpsValid = 1;

    left_rear_ws.wheelCountValid = 0;
    right_rear_ws.wheelCountValid = 0;
    left_front_ws.wheelCountValid = 0;
    right_front_ws.wheelCountValid = 0;

    left_rear_ws.wheelSpeedPos = 2;
    right_rear_ws.wheelSpeedPos = 3;
    left_front_ws.wheelSpeedPos = 0;
    right_front_ws.wheelSpeedPos = 1;

    left_rear_ws.wheelSpeedMps = wheel_speed_data_ptr->left_rear_speed;
    right_rear_ws.wheelSpeedMps = wheel_speed_data_ptr->right_rear_speed;
    left_front_ws.wheelSpeedMps = wheel_speed_data_ptr->left_front_speed;
    right_front_ws.wheelSpeedMps = wheel_speed_data_ptr->right_front_speed;

    msg->wheel_speed_vec.push_back(left_rear_ws);
    msg->wheel_speed_vec.push_back(right_rear_ws);
    msg->wheel_speed_vec.push_back(left_front_ws);
    msg->wheel_speed_vec.push_back(right_front_ws);
    // msg->pluse_mask = msg->header.seq;

    // if (driver_ptr->debug_ & 2048) {
    //     printf(
    //         "wheel_speed's header_ts[%.3f] week[%d], utc_time[%.3f], left_rear_speed[%.5f], right_rear_speed[%.5f],
    //         left_front_speed[%.5f], right_front_speed[%.5f], pluse_mask[%d]\n", msg->header.time, msg->week,
    //         msg->utc_time, msg->left_rear_speed, msg->right_rear_speed, msg->left_front_speed,
    //         msg->right_front_speed, msg->pluse_mask);
    // }
    return 0;
}


int AdsfiInterface::Process(const std::string &name, std::shared_ptr<ara::adsfi::MsgHafIMU> &msg){
    auto change_ptr = BD21DataInteract::Instance();
    change_ptr->GetImuData(*sensor_imu_data_ptr);
    /********** imu ******************/
    double t = NavboxUtc2Unix(sensor_imu_data_ptr->week, sensor_imu_data_ptr->utc_time);
    msg->imuHeader.seq = imu_seq++;
    msg->imuHeader.frameID = "/base_link";
    msg->imuHeader.timestamp.sec = ADSFI_GET_SEC(t);
    msg->imuHeader.timestamp.nsec = ADSFI_GET_NSEC(t);

    msg->angularVelocity.x = sensor_imu_data_ptr->gyro_x;
    msg->angularVelocity.y = sensor_imu_data_ptr->gyro_y;
    msg->angularVelocity.z = sensor_imu_data_ptr->gyro_z;
    // accelermeter m/s^2
    msg->linearAcceleration.x = sensor_imu_data_ptr->acce_x;
    msg->linearAcceleration.y = sensor_imu_data_ptr->acce_y;
    msg->linearAcceleration.z = sensor_imu_data_ptr->acce_z;

    //unsigned short	IMU状态信息
    // msg->imuStatus = ;
    msg->temperature = sensor_imu_data_ptr->temperature;

    return 0;
}
