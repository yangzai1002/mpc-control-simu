#pragma once

#include "ara/adsfi/impl_type_businesscommand.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/vehicle_status.h"
#include "xkit/planning/datapool/planning_info.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "PlanningTrajectory.h"

void XLevelingAndHeadingAdjustment ( const ara::adsfi::BusinessCommand& business_command , 	// 业务信息	
const avos::planning::LocalizationData& localization_data , 	// 定位信息	
const avos::planning::Perception& perception , 	// 感知数据	
const avos::planning::VehicleStatus& vehicle_status , 	// 车辆状态信息	
const avos::planning::PlanningInfo& planning_info_in , 	// 输入规划内部信息	
const ara::adsfi::PerceptionStaticEnv& semanticmap , 	// 语义地图数据	
PlanningTrajectory& pathplan , 	// 规划轨迹信息	
avos::planning::PlanningInfo& planning_info_out , 	// 输出规划内部信息	
ara::adsfi::PerceptionStaticEnv& terrain_debug );
