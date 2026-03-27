#include <chrono>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "DatetimeUtil.hpp"
#include "EC400.hpp"
#include "VideoEditYUV420UV12.hpp"
#include "VideoParameter.h"
#include "ap_log/ap_log_interface.h"
#include "ara/adsfi/impl_type_msghafegotrajectory.h"
#include "ara/adsfi/impl_type_msghaffusionoutarray.h"
#include "ara/adsfi/impl_type_msgimagedata.h"
#include "custom_stack.h"
#include "fmt/format.h"
#include "opencv2/freetype.hpp"
#include "shm_util/zero_copy_shm_mem.h"
#include "time_cost.hpp"

// 新增功能模块头文件
#include "../XDrawReverseWarning/XDrawReverseWarning.h"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080
#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

// 目标对象包装结构
struct TargetObject {
    int                                priority;  // 优先级：1-壕沟, 2-车辆, 3-行人, 4-箱体
    cv::Rect                           rect;      // 矩形框
    std::string                        type_name; // 类型名称
    const ara::adsfi::MsgHafFusionOut *obj_ptr;   // 原始对象指针
};

// 矩形相交判断（边界接触不算交集）
bool isRectIntersect(const cv::Rect &rect1, const cv::Rect &rect2) {
    // 使用 < 而不是 <= 来排除边界接触的情况
    return !(rect1.x + rect1.width <= rect2.x ||  // rect1在rect2左边
             rect2.x + rect2.width <= rect1.x ||  // rect1在rect2右边
             rect1.y + rect1.height <= rect2.y || // rect1在rect2上边
             rect2.y + rect2.height <= rect1.y);  // rect1在rect2下边
}

// 检查目标是否与高优先级目标相交
bool hasIntersectionWithHigherPriority(const cv::Rect &                 target_rect,
                                       const std::vector<TargetObject> &higher_priority_objects) {
    for (const auto &obj : higher_priority_objects) {
        if (isRectIntersect(target_rect, obj.rect)) {
            return true;
        }
    }
    return false;
}

// 获取目标优先级
int getObjectPriority(int fusionType) {
    if (fusionType == 27) {
        return 1; // 壕沟
    } else if (fusionType == 2 || fusionType == 3 || fusionType == 4 || fusionType == 6) {
        return 2; // 车辆
    } else if (fusionType == 0 || fusionType == 1) {
        return 3; // 行人
    } else if (fusionType == 26) {
        return 4; // 箱体
    }
    return -1; // 未知类型
}

// 获取目标类型名称
std::string getObjectTypeName(int fusionType) {
    if (fusionType == 0 || fusionType == 1) {
        return "人员";
    } else if (fusionType == 2 || fusionType == 3 || fusionType == 4 || fusionType == 6) {
        return "车辆";
    } else if (fusionType == 26) {
        return "箱体";
    } else if (fusionType == 27) {
        return "壕沟";
    }
    return "未知";
}

//绘制识别框和文字
void drawTextYUV420SP(uint8_t *yuv, int width, int height, cv::Ptr<cv::freetype::FreeType2> ft2,
                      const std::string &text, cv::Point pos, int fontSize = 50) {
    if (nullptr == yuv) {
        ApError("video_editor") << "yuv is nullptr";
        return;
    }
    if (nullptr == ft2) {
        ApError("video_editor") << "freetype2 is nullptr";
        return;
    }
    if (text.empty()) {
        ApError("video_editor") << "text is empty";
        return;
    }
    if (width <= 0 || height <= 0 || (width & 1) || (height & 1)) {
        ApError("video_editor") << "error, width: " << width << ", height: " << height;
        return;
    }

    int      baseline = 0;
    cv::Size textSize;

    try {
        textSize = ft2->getTextSize(text, fontSize, -1, &baseline);
    } catch (const std::exception &e) {
        ApError("video_editor") << "getTextSize error: " << e.what();
        return;
    } catch (...) {
        ApError("video_editor") << "getTextSize error";
        return;
    }

    if (textSize.width <= 0 || textSize.height <= 0) {
        ApError("video_editor") << "error textsize, width: " << textSize.width << ", height: " << textSize.height;
        return;
    }

    // === 计算有效绘制区域 ===
    int startX = std::max(0, pos.x);
    int startY = std::max(0, pos.y);
    int endX = std::min(width, pos.x + textSize.width);
    int endY = std::min(height, pos.y + textSize.height);

    if (startX >= endX || startY >= endY) {
        ApError("video_editor") << "error startX or startY, startX: " << startX << ", startY: " << startY
                                << ", endX: " << endX << ", endY: " << endY;
        return;
    }

    // === 生成灰度 mask（直接单通道） ===
    cv::Mat mask(textSize.height, textSize.width, CV_8UC3, cv::Scalar(0, 0, 0));
    try {
        ft2->putText(mask, text, cv::Point(0, textSize.height), fontSize, cv::Scalar(255, 255, 255), -1, cv::LINE_AA,
                     true);
    } catch (const std::exception &e) {
        ApError("video_editor") << "putText error: " << e.what();
        return;
    } catch (...) {
        ApError("video_editor") << "draw text error";
        return;
    }

    cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);

    uint8_t *Y = yuv;
    uint8_t *UV = yuv + width * height;

    int draw_w = std::min(textSize.width, width - pos.x);
    int draw_h = std::min(textSize.height, height - pos.y);
    if (draw_w <= 0 || draw_h <= 0) {
        ApError("video_editor") << "error draw_w or draw_h, draw_w: " << draw_w << ", draw_h: " << draw_h;
        return;
    }

    const uint8_t Yc = 76;
    const uint8_t Uc = 84;
    const uint8_t Vc = 255;

    for (int j = 0; j < draw_h; j++) {
        int yy = pos.y + j;
        if (yy < 0 || yy >= height)
            continue;
        const uint8_t *mask_row = mask.ptr<uint8_t>(j);
        for (int i = 0; i < draw_w; i++) {
            int xx = pos.x + i;
            if (xx < 0 || xx >= width)
                continue;
            uint8_t alpha = mask_row[i];
            if (alpha == 0)
                continue;
            Y[yy * width + xx] = Yc;
            if ((yy % 2 == 0) && (xx % 2 == 0)) {
                int uvIndex = (yy / 2) * width + xx;
                if (uvIndex >= 0 && uvIndex + 1 < width * height / 2) {
                    UV[uvIndex] = Uc;
                    UV[uvIndex + 1] = Vc;
                }
            }
        }
    }
}

// 过滤并生成待绘制列表（按优先级过滤）
std::vector<TargetObject> filterObjectsByPriority(const ara::adsfi::MsgHafFusionOutArray &objects) {
    // 优先级分组
    std::vector<TargetObject> trenches;    // Priority 1 - 壕沟
    std::vector<TargetObject> vehicles;    // Priority 2 - 车辆
    std::vector<TargetObject> pedestrians; // Priority 3 - 行人
    std::vector<TargetObject> boxes;       // Priority 4 - 箱体

    // 1. 数据预处理：按优先级分类
    for (const auto &obj : objects.fusionOut) {
        if (obj.rect_2d.corners.size() < 2) {
            continue;
        }

        int priority = getObjectPriority(obj.fusionType);
        if (priority == -1) {
            continue; // 跳过未知类型
        }

        auto &   uv = obj.rect_2d.corners[0];
        auto &   wh = obj.rect_2d.corners[1];
        cv::Rect rect(cv::Point(uv.x, uv.y), cv::Point(wh.x, wh.y));

        // 检查矩形有效性
        if (rect.width <= 0 || rect.height <= 0) {
            continue;
        }

        TargetObject target;
        target.priority = priority;
        target.rect = rect;
        target.type_name = getObjectTypeName(obj.fusionType);
        target.obj_ptr = &obj;

        // 按优先级分组
        if (priority == 1) {
            trenches.push_back(target);
        } else if (priority == 2) {
            vehicles.push_back(target);
        } else if (priority == 3) {
            pedestrians.push_back(target);
        } else if (priority == 4) {
            boxes.push_back(target);
        }
    }

    // 2. 过滤阶段：按优先级从高到低处理
    std::vector<TargetObject> objects_to_draw;
    objects_to_draw.reserve(trenches.size() + vehicles.size() + pedestrians.size() + boxes.size());

    // 第一轮：壕沟框（最高优先级，全部保留）
    for (const auto &trench : trenches) {
        objects_to_draw.push_back(trench);
    }

    // 第二轮：车辆框（过滤与壕沟框相交的）
    for (const auto &vehicle : vehicles) {
        if (!hasIntersectionWithHigherPriority(vehicle.rect, trenches)) {
            objects_to_draw.push_back(vehicle);
        } else {
            ApInfo("video_editor") << "Filtered vehicle: rect=[" << vehicle.rect.x << "," << vehicle.rect.y << ","
                                   << vehicle.rect.width << "," << vehicle.rect.height
                                   << "] (intersects with trench)";
        }
    }

    // 第三轮：行人框（过滤与壕沟框、车辆框相交的）
    std::vector<TargetObject> higher_priority_for_pedestrians;
    higher_priority_for_pedestrians.reserve(trenches.size() + vehicles.size());
    for (const auto &obj : objects_to_draw) {
        if (obj.priority <= 2) { // 壕沟和车辆
            higher_priority_for_pedestrians.push_back(obj);
        }
    }

    for (const auto &pedestrian : pedestrians) {
        if (!hasIntersectionWithHigherPriority(pedestrian.rect, higher_priority_for_pedestrians)) {
            objects_to_draw.push_back(pedestrian);
        } else {
            ApInfo("video_editor") << "Filtered pedestrian: rect=[" << pedestrian.rect.x << "," << pedestrian.rect.y
                                   << "," << pedestrian.rect.width << "," << pedestrian.rect.height
                                   << "] (intersects with higher priority)";
        }
    }

    // 第四轮：箱体框（过滤与壕沟框、车辆框、行人框相交的）
    std::vector<TargetObject> higher_priority_for_boxes;
    higher_priority_for_boxes.reserve(objects_to_draw.size());
    for (const auto &obj : objects_to_draw) {
        if (obj.priority <= 3) { // 壕沟、车辆和行人
            higher_priority_for_boxes.push_back(obj);
        }
    }

    for (const auto &box : boxes) {
        if (!hasIntersectionWithHigherPriority(box.rect, higher_priority_for_boxes)) {
            objects_to_draw.push_back(box);
        } else {
            ApInfo("video_editor") << "Filtered box: rect=[" << box.rect.x << "," << box.rect.y << "," << box.rect.width
                                   << "," << box.rect.height << "] (intersects with higher priority)";
        }
    }

    // 日志统计
    ApInfo("video_editor") << "Priority filtering: Total=" << objects.fusionOut.size()
                           << ", Trenches=" << trenches.size() << ", Vehicles=" << vehicles.size()
                           << ", Pedestrians=" << pedestrians.size() << ", Boxes=" << boxes.size()
                           << ", After filtering=" << objects_to_draw.size();

    return objects_to_draw;
}

// 绘制过滤后的目标信息
void drawTargetInfo(void *data, cv::Ptr<cv::freetype::FreeType2> ft2, const int &width, const int &height,
                    const std::string &color, const int &fontSize, const ara::adsfi::MsgHafFusionOutArray &objects) {
    if (data == nullptr) {
        ApError("video_editor") << "drawTargetInfo error: data is nullptr";
        return;
    }

    // 执行优先级过滤
    std::vector<TargetObject> objects_to_draw = filterObjectsByPriority(objects);

    // 绘制过滤后的目标框
    const int thickness = 2;
    for (const auto &target : objects_to_draw) {
        cv::Point top_left(target.rect.x, target.rect.y);
        cv::Point bottom_right(target.rect.x + target.rect.width, target.rect.y + target.rect.height);

        ApInfo("video_editor") << "Drawing " << target.type_name << ": rect=[" << target.rect.x << "," << target.rect.y
                               << "," << target.rect.width << "," << target.rect.height << "]";

        // 绘制文字标签
        cv::Point text_pos(target.rect.x, target.rect.y - fontSize);
        drawTextYUV420SP((uint8_t *)data, width, height, ft2, target.type_name, text_pos, fontSize);

        // 绘制矩形框
        VideoEditYUV420UV12::drawRectangle(data, width, height, color, top_left, bottom_right, thickness);
    }
}

bool XDrawObstacles(const ara::adsfi::MsgImageData &  image_in, // 前相机参数
                    const compatible::VideoParameter &parame,   // 视频绘制参数
                    ara::adsfi::MsgImageData &        image_out) {
    ApInfo("video_editor") << "XDrawObstacles start";
    ApInfo("video_editor") << fmt::format("XDrawObstacles input image name: {}, time: {:20.8f}", image_in.frameID,
                                          app_common::DatetimeUtil::double_time_from_sec_nsec(image_in.timestamp));

    static bool                             init{false};
    static cv::Ptr<cv::freetype::FreeType2> ft2{nullptr};
    static std::string                      color{"red"};
    static int                              foneSize{50};
    static ZeroCopyShmMem                   zero_shm_mem_image;

    //判断数据有效性
    if (image_in.height <= 0 || image_in.width <= 0) {
        ApError("video_editor") << "XDrawObstacles data is invalid";
        EC400::Instance()->ec_add(EC400::DATA_INVALID);
        return false;
    }
    EC400::Instance()->ec_remove(EC400::DATA_INVALID);

    if (!init) {
        ft2 = cv::freetype::createFreeType2();
        auto ptr = CustomStack::Instance();
        auto fontPath = ptr->GetProjectConfigPath() + "/" + "wqy-microhei.ttc";
        if (!std::filesystem::exists(fontPath)) {
            EC400::Instance()->ec_add(EC400::CONFIG);
        }
        ft2->loadFontData(fontPath, 0);

        std::string prefix;
        auto        target_color_path = prefix + "video/edit/target_info_color";
        if (!ptr->GetProjectConfigValue(target_color_path, color)) {
            EC400::Instance()->ec_add(EC400::CONFIG, target_color_path);
            throw std::runtime_error("read project config " + target_color_path + " failed");
        }

        auto target_text_path = prefix + "video/edit/target_text_size";
        if (!ptr->GetProjectConfigValue(target_text_path, foneSize)) {
            EC400::Instance()->ec_add(EC400::CONFIG, target_text_path);
            throw std::runtime_error("read project config " + target_text_path + " failed");
        }

        EC400::Instance()->ec_remove(EC400::CONFIG);

        init = true;
    }

    //处理前相机数据
    auto shm_front_ptr = zero_shm_mem_image.GetAllocShmMem(
        image_in.frameID, VIDEO_OUPUT_SHM_MAX_WIDTH * VIDEO_OUPUT_SHM_MAX_HEIGHT * 3 / 2, image_in.seq);
    ApInfo("video_editor") << "XDrawObstacles color= " << color << ", foneSize=" << foneSize;
    if (image_in.frameID == "put_front") {
        drawTargetInfo((void *)shm_front_ptr, ft2, image_in.width, image_in.height, color, foneSize,
                       parame.editor_parameter.objects);
    }

    image_out = image_in;
    
#    // ========== 新增功能：倒车预警 ==========
#    // 仅在后视相机时生效（内部判断frameID == "put_back"）
#    //XDrawReverseWarning(image_out, parame.editor_parameter.objects);
#    // ========== 新增功能调用结束 ==========
    
    ApInfo("video_editor") << "XDrawObstacles end";
    return true;
}
