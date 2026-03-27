#pragma once

#include "xkit/planning/datapool/vehicle_status.h"
#include "xkit/planning/datapool/planning_info.h"
#include "xkit/planning/datapool/planning_info.h"
#include "ara/adsfi/impl_type_planningstatus.h"

void XBusinessFollow ( const avos::planning::VehicleStatus& vehicle_status , 	// 车辆状态	
const avos::planning::PlanningInfo& planning_info_in , 	// 输入规划信息	
avos::planning::PlanningInfo& planning_info_out , 	// 规划相关信息	
ara::adsfi::PlanningStatus& mission_status );
