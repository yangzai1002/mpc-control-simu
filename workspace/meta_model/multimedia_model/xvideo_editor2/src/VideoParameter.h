#pragma once

#include "ara/adsfi/impl_type_appvideosplitparam.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"

namespace compatible {
    struct VideoSplitPoint {
        int left_top_point_x;     // 左上点x
        int left_top_point_y;     // 左上点y
        int right_bottom_point_x; // 右下点x
        int right_bottom_point_y; // 右下点y
    };

    struct VideoSplitParam {
        std::string     channel_name;     // 相机名称
        VideoSplitPoint view_area_points; // 显示区域点信息
        VideoSplitPoint crop_area_points; // 裁剪区域点信息
    };

    struct VideoEditorParameter {
        int output_mode;           // 1-全彩；2-灰度；3-二值；4-特征图
        int remote_control_alarm;  // 是否打开遥控预警（0-关闭；1-打开）
        int remote_control_assist; // 是否打开遥控辅助（0-关闭；1-打开）
        int front_source;          // 前相机数据源（0-前视相机；4-前视微光）
        int split_adaptive;        // 是否自适应切换视频内容（0-关闭；1-打开）
        int video_left_;           // 0-前视；1-为左前；2-右前；3-后视； 4-前视微光；-1-无
        int video_main_;           // 0-前视；1-为左前；2-右前；3-后视；  4-前视微光；-1-无
        int video_right_;          // 0-前视；1-为左前；2-右前；3-后视； 4-前视微光； -1-无
        ara::adsfi::MsgHafFusionOutArray objects;        // 目标识别结果
        ara::adsfi::MsgHafEgoTrajectory  remote_control; // 遥控时的路径规划
        ara::adsfi::MsgHafEgoTrajectory  path;           // 路径规划（循迹、跟随等 ）
        int video_third_person; // 是否开启第三人称（上帝）视角（0-关闭；1-打开）
        std::vector<ara::adsfi::AppVideoSplitParam> split_parames_; // 相机分割参数列表
    };

    struct VideoEncodingParameter {
        int    adaptive_mode;     // 自动调整码率和分辨率（0-关闭；1-打开）
        int    rate_control_mode; // 码率控制模式：0-码率；1-质量；
        int    quality;           // 编码质量：1-51，1 最好，51 最差
        int    bps;               // 输出码率（kbps）
        int    fps;               // 输出帧率
        int    encoding_gop;      // 编码器GOP
        double width;             // 输出宽度
        double height;            // 输出高度
    };

    struct VideoParameter {
        VideoEditorParameter   editor_parameter;   // 视频编辑参数
        VideoEncodingParameter encoding_parameter; // 视频编码参数
    };
} // namespace compatible