
#include "controller_node.hpp"

namespace control
{
void  Controller::init()
{
    // (1) PP横向控制器
    m_lateralController_pursuit = std::make_shared<
            pure_pursuit::PurePursuitLateralController>(
                    "/opt/usr/zjy/config/control/lateral_controller_pursuit.yaml");

    // (2) PID纵向控制器
    m_longitudinalController_ = std::make_shared<
            pid_longitudinal_controller::PidLongitudinalController>(
                    "/opt/usr/zjy/config/control/longitudinal_controller_pid.yaml");

    m_lateralController_mpc = std::make_shared<
            MpcFollower::MPCFollowerController>(
                    "/opt/usr/zjy/config/control/lateral_controller_mpc.yaml");

              
        //return m_controlNode.Init()
}

//
void Controller::stop()
{
    //m_controlNode.Stop();
}


// 把计算得到的控制指令发送到底盘驱动
void Controller::sendDrivingCmd(LateralOutput lat_cmd_in, LongitudinalOutput lon_cmd_in)
{
    auto chassisCmd = std::make_shared<HafChassisCommand>();

    timeval tv;
    gettimeofday(&tv, nullptr);
    const uint32_t usScaler = 1000U;
    chassisCmd->chassisCmdHeader.timestamp.sec = tv.tv_sec;
    chassisCmd->chassisCmdHeader.timestamp.nsec = tv.tv_usec * usScaler;

    //方向盘转角和前面两组轮子中心转角之间标定的比例: 25.713
    chassisCmd->steerAngleCmd = -lat_cmd_in.control_cmd.steering_tire_angle* STEERING_RATIO;


    // 横向控制命令
    HAF_LOG_INFO
    << " lat_cmd [steering_tire_angle: " << lat_cmd_in.control_cmd.steering_tire_angle
    << " [rad], steerAngleCmd: " << chassisCmd->steerAngleCmd
    << " [rad] = " << chassisCmd->steerAngleCmd/M_PI*180.0
    << " [deg], steering_tire_rotation_rate: "<< lat_cmd_in.control_cmd.steering_tire_rotation_rate
    << ", IsSteerConverged: "<< lat_cmd_in.control_cmd.IsSteerConverged << " ]";

    // 纵向控制命令
    HAF_LOG_INFO
    << " lon_cmd [speed: " << lon_cmd_in.control_cmd.speed << " ,"
    << " acceleration: " << lon_cmd_in.control_cmd.acceleration << ", "
    << " jerk: " << lon_cmd_in.control_cmd.jerk << ", "
    << " throttle: " << lon_cmd_in.control_cmd.throttle << ", "
    << " brake: " << lon_cmd_in.control_cmd.brake << " ]";

    chassisCmd->stopEnable = 0;

    // 速度>0, 前进档
    if (lon_cmd_in.control_cmd.speed > 0)
    {
        chassisCmd->gearCmd.value = commStruct::GearCMD::DRIVE;
    }
    // 目标速度接近0, 驻车档
    else if (std::fabs(lon_cmd_in.control_cmd.speed) < 1e-2) // 速度单位
    {
        chassisCmd->gearCmd.value = commStruct::GearCMD::PARK;
        chassisCmd->stopEnable = 1;
    }
    // 目标速度<0, 倒车档
    else if (lon_cmd_in.control_cmd.speed < 0)
    {
        chassisCmd->gearCmd.value = commStruct::GearCMD::REVERSE;
    }

    chassisCmd->throttlePedalCmd = lon_cmd_in.control_cmd.throttle;
    chassisCmd->longAccelerationCmd = lon_cmd_in.control_cmd.acceleration;
    chassisCmd->brakePedalCmd = lon_cmd_in.control_cmd.brake;
    chassisCmd->speedLimit = lon_cmd_in.control_cmd.speed;  // m/s

    HAF_LOG_INFO << "#### steerAngleCmd: " << chassisCmd->steerAngleCmd
            << ", throttlePedalCmd: " << chassisCmd->throttlePedalCmd
            << ", gearCmd.value: " << chassisCmd->gearCmd.value
            << ", brakePedalCmd: " << chassisCmd->brakePedalCmd;


    // // 发送底盘命令
    // if (m_controlNode.SendChassisCommand(chassisCmd) != HAF_SUCCESS)
    // {
    //     HAF_LOG_INFO << " Send chassis command data failed!";
    // }

    


    //车身命令
    auto bodyCmd = std::make_shared<HafBodyCommand>();

    bodyCmd->bodyCmdHeader.timestamp.sec = tv.tv_sec;
    bodyCmd->bodyCmdHeader.timestamp.nsec = tv.tv_usec * usScaler;

    // if (m_controlNode.SendBodyCommand(bodyCmd) != HAF_SUCCESS)
    // {
    //     HAF_LOG_INFO << " Send body command data failed!";
    // }

}



// 发送停车控制指令
void Controller::sendStopCmd()
{

    auto chassisCmd = std::make_shared<HafChassisCommand>();

    timeval tv;
    gettimeofday(&tv, nullptr);
    const uint32_t usScaler = 1000U;
    chassisCmd->chassisCmdHeader.timestamp.sec = tv.tv_sec;
    chassisCmd->chassisCmdHeader.timestamp.nsec = tv.tv_usec * usScaler;

    chassisCmd->steerAngleCmd = 0;

    // 横向控制命令
    HAF_LOG_INFO << " steerAngleCmd: " << chassisCmd->steerAngleCmd
            << " [rad] = " << chassisCmd->steerAngleCmd / M_PI * 180.0;

    chassisCmd->gearCmd.value = commStruct::GearCMD::PARK;
    chassisCmd->stopEnable = 1;
    chassisCmd->throttlePedalCmd = 0;
    chassisCmd->longAccelerationCmd = 0;
    chassisCmd->brakePedalCmd = 20;

    HAF_LOG_INFO << "#### steerAngleCmd: " << chassisCmd->steerAngleCmd
            << ", throttlePedalCmd: " << chassisCmd->throttlePedalCmd
            << ", gearCmd.value: " << chassisCmd->gearCmd.value
            << ", brakePedalCmd: " << chassisCmd->brakePedalCmd;

    // if (m_controlNode.SendChassisCommand(chassisCmd) != HAF_SUCCESS)
    // {
    //     HAF_LOG_INFO << " Send chassis command data failed!";
    // }

}


void Controller::runControl(std::shared_ptr<ara::adsfi::VehicleActControl>& VehicleActControl)
{
  auto start = std::chrono::system_clock::now();
 
  
  
    static uint64_t loop_count = 1;
    HAF_LOG_INFO << "======================================= loop_count: "
            << loop_count << " =======================================";

   // processInputData();

    const bool is_lat_ready = m_lateralController_mpc->isReady(m_inputData);
    const bool is_lon_ready = m_longitudinalController_->isReady(m_inputData);

    if (!is_lat_ready || !is_lon_ready)
    {
        HAF_LOG_INFO << " ### [Lat/Lon controller is not ready to run ] or [Recv Stop CMD]";

        return;
    }


    const auto lat_cmd = m_lateralController_mpc->run(m_inputData);
    const auto lon_cmd = m_longitudinalController_->run(m_inputData);

    //VehicleActControl->lat_control.target_angle = lat_cmd.control_cmd.steering_tire_angle;
    VehicleActControl->lat_control.target_angle = lat_cmd.control_cmd.steering_tire_rotation_rate;
    VehicleActControl->lon_control.target_speed = lon_cmd.control_cmd.speed;

    m_longitudinalController_->sync(lat_cmd.sync_data);

    m_lateralController_mpc->sync(lon_cmd.sync_data);

    //sendDrivingCmd(lat_cmd, lon_cmd);

    loop_count ++;

   double elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count() * 1.0e-6;
   printf("[lateralController] timerCallback: lateralController calculating time = %f [ms]\n",elapsed_ms);
   printf("[lateralController] lat_cmd = %f,lon_cmd = %f\n",lat_cmd.control_cmd.steering_tire_rotation_rate,lon_cmd.control_cmd.speed);
}
//#define Gazebo
// 更新定位信息
void Controller::updateLocation(std::shared_ptr<ara::adsfi::MsgHafLocation>  curlocation)
{
    HAF_LOG_INFO << "### curloc (x, y, z): "
            << curlocation->pose.pose.position.x << ", "
            << curlocation->pose.pose.position.y << ", "
            << curlocation->pose.pose.position.z;

    // 位置
    m_inputData.current_pose.position.x = curlocation->pose.pose.position.x - m_inputData.first_point_x;
    m_inputData.current_pose.position.y = curlocation->pose.pose.position.y - m_inputData.first_point_y;
    m_inputData.current_pose.position.z = curlocation->pose.pose.position.z;
 
// 2. 弧度归一化到 [-π, π]

    const double PI = 3.141592653589793;
    const double TWO_PI = 2 * PI;
    auto normalize = [PI, TWO_PI](double rad) {
        // 手工取模 2π，处理正负
        while (rad < 0)    rad += TWO_PI;
        while (rad > PI)   rad -= TWO_PI;
        return rad;
    };

    /*角度转弧度*/
    #ifndef Gazebo
    m_inputData.current_pose.orientation.x = normalize(curlocation->pose.pose.orientation.x *PI / 180.0);
    m_inputData.current_pose.orientation.y = normalize(curlocation->pose.pose.orientation.y *PI / 180.0);
    m_inputData.current_pose.orientation.z = PI/2 - normalize(curlocation->pose.pose.orientation.z *PI / 180.0); /*将角度转到车体坐标系*/
    #else
    m_inputData.current_pose.orientation.x = curlocation->pose.pose.orientation.x;
    m_inputData.current_pose.orientation.y = curlocation->pose.pose.orientation.y;
    m_inputData.current_pose.orientation.z = curlocation->pose.pose.orientation.z;
    m_inputData.current_twist.linear.x  = curlocation->velocity.twist.linear.x;
    m_inputData.current_twist.linear.y  = curlocation->velocity.twist.linear.y;
    m_inputData.current_twist.linear.z  = curlocation->velocity.twist.linear.z;
    m_inputData.current_chassis_report.velocity = curlocation->velocity.twist.linear.x;
    #endif
    //m_inputData.current_pose.orientation.w = curlocation->pose.pose.orientation.w;

    // 加速度
    m_inputData.current_accel = sqrt( pow(curlocation->acceleration.accel.linear.x, 2)
            + pow(curlocation->acceleration.accel.linear.y, 2)
            + pow(curlocation->acceleration.accel.linear.z, 2) );

}
//车辆的速度和角速度信息从这里更新
void Controller::updateVehicleInfo(std::shared_ptr<ara::adsfi::VehicleInformation> vehicleInfo)
{
    m_inputData.current_chassis_report.velocity = vehicleInfo->vehicle_act_state.speed;
}


void Controller::updateTrajectory(std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> PlanningResult)
{
    HAF_LOG_INFO << "$$$$ curTrajectory: " << PlanningResult->trajectoryPoints.size();
    HAF_LOG_INFO << "$$$$ curwayPoints: " << PlanningResult->wayPoints.size();
    m_inputData.current_trajectory.trajectoryPoints.clear();
    for(int i =0;i< PlanningResult->wayPoints.size()-2;i++)
    {
        HafTrajectoryPoint new_TrajectoryPoint;
        new_TrajectoryPoint.wayPoint.x = PlanningResult->wayPoints[i].x;
        new_TrajectoryPoint.wayPoint.y = PlanningResult->wayPoints[i].y;
        //new_TrajectoryPoint.wayPoint.z = old_point.z;
        new_TrajectoryPoint.wayPoint.theta = PlanningResult->wayPoints[i].theta;
        new_TrajectoryPoint.wayPoint.curvature = PlanningResult->wayPoints[i].curvature;
        new_TrajectoryPoint.speed = 1.5;
    
        m_inputData.current_trajectory.trajectoryPoints.push_back(new_TrajectoryPoint);
    }
    if(m_inputData.current_trajectory.trajectoryPoints.size() > 5)
    {
        m_inputData.current_trajectory.trajectoryPoints[m_inputData.current_trajectory.trajectoryPoints.size()-1].speed = 0.0;
    }
    m_inputData.first_point_x = PlanningResult->wayPoints.back().x;
    m_inputData.first_point_y = PlanningResult->wayPoints.back().y;
    
    std::cout << "$$$$ trajectoryPoints: " << m_inputData.current_trajectory.trajectoryPoints.size();
}


void Controller::updateChassisReport(HafChassisReport chassisReport)
{
    m_inputData.current_chassis_report = chassisReport;
    m_inputData.current_steering.steering_tire_angle = chassisReport.steerAngle/STEERING_RATIO;
}


// 处理接收数据
void Controller::processInputData()
{
    // auto locationDataContainer = m_controlNode.GetLocation(1); //HafLocation
    // auto egoTrajectoryDataContainer = m_controlNode.GetEgoTrajectory(1); //HafEgoTrajectory
    // auto bodyReportContainer = m_controlNode.GetBodyReport(1); //HafBodyReport
    // auto chassisReportDataContainer = m_controlNode.GetChassisReport(1); //HafChassisReport

    // if (!locationDataContainer.empty())
    // {
        //auto location = locationDataContainer.front();
        //updateLocation(*location);
       // locationDataContainer.clear();
   // }


    // if (!egoTrajectoryDataContainer.empty())
    // {
    //     auto egoTrajectory = egoTrajectoryDataContainer.front();
    //     updateTrajectory(*egoTrajectory);
    //     egoTrajectoryDataContainer.clear();
    // }


    // if (!bodyReportContainer.empty())
    // {
    //     auto bodyReport = bodyReportContainer.front();
    //     bodyReportContainer.clear();
    // }


    // if (!chassisReportDataContainer.empty())
    // {
    //     auto chassisReport = chassisReportDataContainer.front();
    //     updateChassisReport(*chassisReport);
    //     chassisReportDataContainer.clear();
    // }

}

}
