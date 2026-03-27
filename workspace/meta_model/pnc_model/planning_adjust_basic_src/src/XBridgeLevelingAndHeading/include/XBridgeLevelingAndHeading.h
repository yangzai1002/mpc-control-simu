#pragma once

#include "ara/adsfi/impl_type_businesscommand.h"
#include "xkit/planning/datapool/localization_input.h"
#include "PerceptionFinalObjects.h"
#include "xkit/planning/datapool/planning_info.h"

void XBridgeLevelingAndHeading ( const ara::adsfi::BusinessCommand& app_cmd , 	// 业务层指令	
const avos::planning::LocalizationData& localization_data , 	// 定位信息	
const PerceptionFinalObjects& perception_obj , 	// 感知融合障碍物信息	
const avos::planning::PlanningInfo& planning_info_in , 	// 输入规划信息	
avos::planning::PlanningInfo& pln_info_out );
