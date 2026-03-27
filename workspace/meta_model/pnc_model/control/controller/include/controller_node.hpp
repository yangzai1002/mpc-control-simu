#ifndef CONTROLLER_NODE_HPP_
#define CONTROLLER_NODE_HPP_

#include "comm/comm_struct.hpp"
#include "trajectory_follower_base/lateral_controller_base.hpp"
#include "trajectory_follower_base/longitudinal_controller_base.hpp"
#include "pid_longitudinal_controller/pid_longitudinal_controller.hpp"
#include "pure_pursuit/pure_pursuit_lateral_controller.hpp"
#include "mpc_lateral_controller/mpc_follower_core.hpp"
#include "control/chassis_command.h"
#include "control/body_command.h"
#include "core/logger.h"
#include "ara/adsfi/impl_type_vehicleactcontrol.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_appremotecontrol.h"
//#include "PlanningTrajectoryPoint.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_businesscommand.h"

using namespace Adsfi;

namespace control
{

using trajectory_follower::LateralOutput;
using trajectory_follower::LongitudinalOutput;


class Controller
{
public:
    Controller()
    {
    }
    ;

    ~Controller()
    {
    }

    void init();
    void stop();

    void updateLocation(std::shared_ptr<ara::adsfi::MsgHafLocation>  curlocation);

    void updateTrajectory(std::shared_ptr<ara::adsfi::MsgHafEgoTrajectory> PlanningResult);

    void updateVehicleInfo(std::shared_ptr<ara::adsfi::VehicleInformation> vehicleInfo);
    
    void updateChassisReport(HafChassisReport chassisReport);

    void processInputData();
    void runControl(std::shared_ptr<ara::adsfi::VehicleActControl>& VehicleActControl);

    void sendDrivingCmd(LateralOutput lat_cmd, LongitudinalOutput lon_cmd);
    void sendStopCmd();
    

private:
    std::shared_ptr<trajectory_follower::LongitudinalControllerBase> m_longitudinalController_;
    std::shared_ptr<trajectory_follower::LateralControllerBase> m_lateralController_pursuit;
    std::shared_ptr<trajectory_follower::LateralControllerBase> m_lateralController_mpc;
    trajectory_follower::InputData  m_inputData;

   // Adsfi::ControlBase m_controlNode;

};

}  // namespace

#endif  //
