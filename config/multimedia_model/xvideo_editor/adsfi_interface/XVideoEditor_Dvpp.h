#pragma once
#include "ara/adsfi/impl_type_idpsensorimage.h"
#include "ara/adsfi/impl_type_idpsensorimagelist.h"
#include "ara/adsfi/impl_type_idpappvideoparameter.h"
#include "ara/adsfi/impl_type_idpvideoencodingperf.h"

void XVideoEditor_Dvpp(const ara::adsfi::IdpSensorImageList &video_list, // 视频组
                       const ara::adsfi::IdpSensorImage &video_data,     // 透明底盘数据
                       const ara::adsfi::IdpAppVideoParameter &parame,   // 切换参数
                       ara::adsfi::IdpSensorImage &image);
