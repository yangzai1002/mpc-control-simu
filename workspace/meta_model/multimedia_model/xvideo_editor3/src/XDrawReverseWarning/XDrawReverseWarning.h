#ifndef XDRAW_REVERSE_WARNING_H
#define XDRAW_REVERSE_WARNING_H

#include "ara/adsfi/impl_type_msgimagedata.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"

/**
 * @brief 倒车预警功能
 * 
 * 功能说明：
 * 1. 切后视相机时（frameID == "put_back"），检测画面5米范围内的人/车辆/障碍物
 * 2. 在视频右下角叠加红色三角形警示图标（内部感叹号）
 * 3. 无目标时不叠加警告
 * 4. 该功能不依赖开关或配置，满足条件自动触发
 * 
 * @param image_data 图像数据（输入输出）
 * @param objects 融合目标列表
 * @return true表示处理成功
 */
bool XDrawReverseWarning(ara::adsfi::MsgImageData& image_data,
                         const ara::adsfi::MsgHafFusionOutArray& objects);

#endif // XDRAW_REVERSE_WARNING_H
