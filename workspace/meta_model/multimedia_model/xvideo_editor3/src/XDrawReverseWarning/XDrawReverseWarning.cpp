#include "XDrawReverseWarning.h"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include <opencv2/opencv.hpp>

// 警告图标绘制参数
constexpr int WARNING_ICON_SIZE = 60;  // 警告图标大小
constexpr int WARNING_MARGIN = 20;     // 距离右下角的边距

// 相机ID定义
#define CAM_REAR 5

/**
 * @brief 绘制红色三角形警告图标（内部感叹号）
 * @param image YUV420 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @param x 图标中心X坐标
 * @param y 图标中心Y坐标
 */
void drawWarningIcon(uint8_t* image, int width, int height, int x, int y) {
    if (!image || width <= 0 || height <= 0) {
        return;
    }

    // 创建OpenCV Mat包装器（仅Y通道）
    cv::Mat y_plane(height, width, CV_8UC1, image);
    
    // 三角形顶点（等边三角形）
    int half_size = WARNING_ICON_SIZE / 2;
    std::vector<cv::Point> triangle_pts = {
        cv::Point(x, y - half_size),                    // 顶点
        cv::Point(x - half_size, y + half_size),        // 左下
        cv::Point(x + half_size, y + half_size)         // 右下
    };
    
    // 绘制红色三角形边框（YUV中红色Y值约为76）
    cv::polylines(y_plane, triangle_pts, true, cv::Scalar(76), 3);
    
    // 绘制感叹号
    // 感叹号上半部分（竖线）
    int exclaim_top = y - half_size / 3;
    int exclaim_mid = y + half_size / 6;
    cv::line(y_plane, cv::Point(x, exclaim_top), cv::Point(x, exclaim_mid), cv::Scalar(76), 4);
    
    // 感叹号下半部分（点）
    int dot_y = y + half_size / 3;
    cv::circle(y_plane, cv::Point(x, dot_y), 3, cv::Scalar(76), -1);
}

/**
 * @brief 检查目标是否在检测区域内
 * @param obj 检测目标
 * @param detection_rect 检测区域矩形
 * @return true表示在区域内
 */
bool isObjectInDetectionArea(const ara::adsfi::MsgHafFusionOut& obj, const cv::Rect& detection_rect) {
    // 获取目标2D框
    if (obj.rect_2d.corners.size() < 4) {
        return false;
    }
    
    // 计算目标框中心点
    float center_x = obj.rect_2d.center.x;
    float center_y = obj.rect_2d.center.y;
    
    // 判断中心点是否在检测区域内
    return detection_rect.contains(cv::Point(static_cast<int>(center_x), static_cast<int>(center_y)));
}

/**
 * @brief 检查是否需要显示倒车预警
 * @param objects 融合目标列表
 * @param detection_rect 检测区域矩形（从配置读取）
 * @return true表示需要显示警告
 */
bool shouldShowReverseWarning(const ara::adsfi::MsgHafFusionOutArray& objects,
                              const cv::Rect& detection_rect) {
    // 检查是否切换到后视相机（通过车辆状态判断）
    // 这里假设通过档位或其他状态判断，具体实现根据实际接口调整
    
    // 遍历所有目标
    for (const auto& obj : objects.fusionOut) {
        // 检查目标类型：人/车辆/障碍物
        // fusionType: 1=车辆, 7=行人, 其他障碍物类型根据实际定义
        bool is_target_type = (obj.fusionType == 1 || obj.fusionType == 7 || 
                               obj.fusionType == 16 || obj.fusionType == 17);
        
        if (is_target_type && isObjectInDetectionArea(obj, detection_rect)) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief 倒车预警功能主函数
 * @param image_data 图像数据
 * @param objects 融合目标列表
 * @return true表示处理成功
 */
bool XDrawReverseWarning(ara::adsfi::MsgImageData& image_data,
                         const ara::adsfi::MsgHafFusionOutArray& objects) {
    // 仅在后视相机时处理（判断frameID）
    if (image_data.frameID != "put_back") {
        return true;
    }
    
    // 读取检测区域配置
    static bool initialized = false;
    static cv::Rect detection_rect;
    
    if (!initialized) {
        // 从配置文件读取检测区域（默认屏幕下半区）
        int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
        
        auto ptr = CustomStack::Instance();
        std::string prefix = "video/reverse_warning/";
        
        if (!ptr->GetProjectConfigValue(prefix + "detection_x", rect_x)) {
            rect_x = 0;  // 默认从左边开始
        }
        if (!ptr->GetProjectConfigValue(prefix + "detection_y", rect_y)) {
            rect_y = image_data.height / 2;  // 默认下半区
        }
        if (!ptr->GetProjectConfigValue(prefix + "detection_width", rect_w)) {
            rect_w = image_data.width;  // 默认全宽
        }
        if (!ptr->GetProjectConfigValue(prefix + "detection_height", rect_h)) {
            rect_h = image_data.height / 2;  // 默认下半区高度
        }
        
        detection_rect = cv::Rect(rect_x, rect_y, rect_w, rect_h);
        initialized = true;
        
        ApInfo("video_editor") << fmt::format("Reverse warning detection area: x={}, y={}, w={}, h={}",
                                              rect_x, rect_y, rect_w, rect_h);
    }
    
    // 检查是否需要显示警告
    if (!shouldShowReverseWarning(objects, detection_rect)) {
        return true;  // 无目标，不显示警告
    }
    
    // 计算警告图标位置（右下角）
    int icon_x = image_data.width - WARNING_MARGIN - WARNING_ICON_SIZE / 2;
    int icon_y = image_data.height - WARNING_MARGIN - WARNING_ICON_SIZE / 2;
    
    // 绘制警告图标
    drawWarningIcon(image_data.rawData.data(), image_data.width, image_data.height, icon_x, icon_y);
    
    ApInfo("video_editor") << "Reverse warning displayed";
    
    return true;
}
