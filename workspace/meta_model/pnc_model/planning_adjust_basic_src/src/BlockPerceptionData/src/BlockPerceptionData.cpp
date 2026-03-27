#include "ap_log/ap_log_interface.h"
#include "BlockPerceptionData.h"
#include "log/log.h"
#include "base_control/adsfi_struct_adapter.h"

void BlockPerceptionData( const ara::adsfi::PerceptionStaticEnv& perception_semantic_map ,   // 感知数据 
const PerceptionFinalObjects& perception_objects ,  // 感知融合数据   
const ara::adsfi::BusinessCommand& business_cmd ,   // 业务命令 
ara::adsfi::PerceptionStaticEnv& output_perception_semantic_map ,   // 清空后的感知语义地图数据 
PerceptionFinalObjects& output_perception_objects ) {
    auto start = std::chrono::high_resolution_clock::now();
    output_perception_semantic_map = perception_semantic_map;
    output_perception_objects = perception_objects;
    AINFO_IF(FLAGS_enable_log) << "task avoid mode: " << business_cmd.param.task_avoid_mode;
    if (business_cmd.param.task_avoid_mode == 3) {
        output_perception_semantic_map.semantic_objs.clear();
        output_perception_objects.objs.clear();
        AINFO_IF(FLAGS_enable_log) << "Clear all semantic objs";
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto cpu_time_used = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "BlockPerceptionData time used: " << cpu_time_used.count() / 1000.0 << " ms\n";
    return;
}
