#pragma once

#include "ara/adsfi/impl_type_perceptionstaticenv.h"
#include "PerceptionFinalObjects.h"
#include "ara/adsfi/impl_type_businesscommand.h"

void BlockPerceptionData ( const ara::adsfi::PerceptionStaticEnv& perception_semantic_map , 	// 感知数据	
const PerceptionFinalObjects& perception_objects , 	// 感知融合数据	
const ara::adsfi::BusinessCommand& business_cmd , 	// 业务命令	
ara::adsfi::PerceptionStaticEnv& output_perception_semantic_map , 	// 清空后的感知语义地图数据	
PerceptionFinalObjects& output_perception_objects );

