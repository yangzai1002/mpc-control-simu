#pragma once

#include "ara/adsfi/impl_type_businesscommand.h"
#include "ara/adsfi/impl_type_msghaflocation.h"
#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "PerceptionFinalObjects.h"
#include "ara/adsfi/impl_type_vehicleinformation.h"
#include "xkit/planning/datapool/perception_input.h"
#include "xkit/planning/datapool/localization_input.h"
#include "xkit/planning/datapool/vehicle_status.h"

void XBridge ( const ara::adsfi::BusinessCommand& task_cmd , 	// 业务命令	
const ara::adsfi::MsgHafLocation& loc , 	// 定位原始数据	
const ara::adsfi::PerceptionStaticEnv& semanticmap , 	// 语义地图数据	
const PerceptionFinalObjects& perception_objs , 	// 感知融合障碍物数据	
const ara::adsfi::VehicleInformation& car_status , 	// 底盘状态	
avos::planning::Perception& perception , 	// 感知数据	
avos::planning::LocalizationData& localization_data , 	// 定位数据	
avos::planning::VehicleStatus& vehicle_status );
