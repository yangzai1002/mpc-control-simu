#include <chrono>
#include <freetype.hpp>
#include <iostream>
#include <map>
#include <opencv2/core.hpp>
#include <set>
#include <tuple>
#include <unordered_map>

// #include "ara::adsfi::IdpPlanningTrajectoryPoint.h"
#include "XImagePathPlan.h"
#include "XVideoEditor_Dvpp.h"
#include "custom_stack.h"
#include "log/log.h"
#include "params/cam_shm_param.h"
#include "protocol/cusermdc610funcinterface.h"
#include "time_cost.hpp"
#include "zero_copy_shm_mem.h"
#include "ara/adsfi/impl_type_idpsensorimage.h"
#include "ara/adsfi/impl_type_idpsensorimagelist.h"

#define VIDEO_IMAGE_MAX_WIDTH 1920
#define VIDEO_IMAGE_MAX_HEIGHT 1080

#define VIDEO_OUPUT_SHM_MAX_WIDTH 3840
#define VIDEO_OUPUT_SHM_MAX_HEIGHT 2160

#define MAX_UNCHANGED 90


using namespace ara::adsfi;
void drawTextYUV420SP(uint8_t *yuv, int width, int height, cv::Ptr<cv::freetype::FreeType2> ft2,
                      const std::string &text, cv::Point pos, int fontSize = 50)
{
    if (nullptr == yuv)
    {
        AERROR << "yuv is nullptr";
        return;
    }
    // if (nullptr == ft2)
    // {
    //     AERROR << "freetype2 is nullptr";
    //     return;
    // }
    if (text.empty())
    {
        AERROR << "text is empty";
        return;
    }
    if (width <= 0 || height <= 0 || (width & 1) || (height & 1))
    {
        AERROR << "error, width: " << width << ", height: " << height;
        return;
    }

    int baseline = 0;
    cv::Size textSize;

    try
    {
        // textSize = ft2->getTextSize(text, fontSize, -1, &baseline);
        textSize = cv::getTextSize(text,
                                    cv::FONT_HERSHEY_SIMPLEX,
                                    fontSize / 30.0,   // 简单映射
                                    1,                   // thickness
                                    &baseline);
    }
    catch (const std::exception &e)
    {
        AERROR << "getTextSize error: " << e.what();
        return;
    }
    catch (...)
    {
        AERROR << "getTextSize error";
        return;
    }

    if (textSize.width <= 0 || textSize.height <= 0)
    {
        AERROR << "error textsize, width: " << textSize.width << ", height: " << textSize.height;
        return;
    }

    // === 计算有效绘制区域 ===
    int startX = std::max(0, pos.x);
    int startY = std::max(0, pos.y);
    int endX = std::min(width, pos.x + textSize.width);
    int endY = std::min(height, pos.y + textSize.height);

    if (startX >= endX || startY >= endY)
    {
        AERROR << "error startX or startY, startX: " << startX << ", startY: " << startY << ", endX: " << endX
               << ", endY: " << endY;
        return;
    }

    // === 生成灰度 mask（直接单通道） ===
    cv::Mat mask(textSize.height, textSize.width, CV_8UC3, cv::Scalar(0, 0, 0));
    try
    {
        // ft2->putText(mask, text, cv::Point(0, textSize.height), fontSize, cv::Scalar(255, 255, 255), -1, cv::LINE_AA, true);
        cv::putText(mask,
            text,
            cv::Point(0, textSize.height),
            cv::FONT_HERSHEY_SIMPLEX,
            fontSize / 30.0,
            cv::Scalar(255, 255, 255),
            1,                      // thickness = 1（原来 -1 表示填充，原生 putText 用正值）
            cv::LINE_AA);
    }
    catch (const std::exception &e)
    {
        AERROR << "putText error: " << e.what();
        return;
    }
    catch (...)
    {
        AERROR << "draw text error";
        return;
    }

    cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);

    uint8_t *Y = yuv;
    uint8_t *UV = yuv + width * height;

    int draw_w = std::min(textSize.width, width - pos.x);
    int draw_h = std::min(textSize.height, height - pos.y);
    if (draw_w <= 0 || draw_h <= 0)
    {
        AERROR << "error draw_w or draw_h, draw_w: " << draw_w << ", draw_h: " << draw_h;
        return;
    }

    const uint8_t Yc = 76;
    const uint8_t Uc = 84;
    const uint8_t Vc = 255;

    for (int j = 0; j < draw_h; j++)
    {
        int yy = pos.y + j;
        if (yy < 0 || yy >= height)
            continue;
        const uint8_t *mask_row = mask.ptr<uint8_t>(j);
        for (int i = 0; i < draw_w; i++)
        {
            int xx = pos.x + i;
            if (xx < 0 || xx >= width)
                continue;
            uint8_t alpha = mask_row[i];
            if (alpha == 0)
                continue;
            Y[yy * width + xx] = Yc;
            if ((yy % 2 == 0) && (xx % 2 == 0))
            {
                int uvIndex = (yy / 2) * width + xx;
                if (uvIndex >= 0 && uvIndex + 1 < width * height / 2)
                {
                    UV[uvIndex] = Uc;
                    UV[uvIndex + 1] = Vc;
                }
            }
        }
    }
}

void drawTargetInfo(void *data, cv::Ptr<cv::freetype::FreeType2> ft2, const int &width, const int &height,
                    const IdpPerceptionFinalObjects &objects)
{
    // 绘制目标框
    for (auto &obj : objects.objs)
    {
        if (data == nullptr || obj.img_info.rect.size() < 2)
        {
            AINFO << "drawTargetInfo error,img rect size=" << obj.img_info.rect.size();
            continue;
        }

        const int thickness = 2;
        auto &uv = obj.img_info.rect[0];
        auto &wh = obj.img_info.rect[1];

        AINFO << "drawTargetInfo uv.x=" << uv.x << ",uv.y=" << uv.y << ",wh.x=" << wh.x << ",wh.y=" << wh.y;

        cv::Point top_left(uv.x, uv.y);
        cv::Point bottom_right(wh.x, wh.y);
        cv::Rect rect(top_left, bottom_right);

        // std::string color;
        // if (obj.plan_risk_level == 2) {
        //     color = "red";
        // } else if (obj.plan_risk_level == 1) {
        //     color = "yellow";
        // } else if (obj.plan_risk_level == 0) {
        //     color = "blue";
        // } else {
        //     continue;
        // }
        std::string color = "red";
        // 绘制矩形框
        VideoEditYUV420UV12::drawRectangle(data, width, height, color, top_left, bottom_right, thickness);

        // 绘制文字
        std::string type = obj.type == 1 ? "行人" : "车辆";
        if (obj.type == 0 || obj.type == 1)
        {
            type = "人";
        }
        else if (obj.type == 2 || obj.type == 3 || obj.type == 4 || obj.type == 6)
        {
            type = "车";
        }
        else
        {
            type = "未知";
        }
        cv::Point text_pos(uv.x, uv.y - 50);
        drawTextYUV420SP((uint8_t *)data, width, height, ft2, type, text_pos);
    }
}

bool addPathPlanPoint(const IdpPlanningTrajectoryPoint &point, const IdpPlanningTrajectoryPoint &next, cv::Point3f &x_lp,
                      cv::Point3f &x_rp)
{
    // std::string usr_config_path = CONFIG_PATH;
    // AINFO << "usr_config_path = " << usr_config_path << std::endl;

    auto ptr = CustomStack::Instance();
    // ptr->LoadModuleConfig(usr_config_path);
    double x_, l_y_, r_y_, car_height_;
    ptr->GetConfig("XVideoEditor",point.x > 0 ? "front_x" : "back_x", x_);
    ptr->GetConfig("XVideoEditor",point.x > 0 ? "front_l" : "back_l", l_y_);
    ptr->GetConfig("XVideoEditor",point.x > 0 ? "front_r" : "back_r", r_y_);
    ptr->GetConfig("XVideoEditor","car_height_", car_height_);

    double an = (next.y - point.y == 0) ? 0 : (next.x - point.x) / (next.y - point.y);
    int turnPosition = (point.y < 0 && next.y < 0)
                           ? ((next.y - point.y > 0) ? 1 : 2)
                       : (point.y < 0 && next.y > 0)
                           ? 1
                       : (point.y > 0 && next.y > 0) ? ((next.y - point.y > 0) ? 1 : 2)
                       : (point.y > 0 && next.y < 0) ? 2
                                                     : 0;

    double x_l = point.x, y_l = point.y + l_y_;
    double x_r = point.x, y_r = point.y - r_y_;

    if (turnPosition == 2 && next.y - point.y != 0)
    { // 右转
        x_l = point.x + l_y_ * cos(atan(an));
        y_l = point.y - l_y_ * sin(atan(an));
        x_r = point.x - r_y_ * cos(atan(an));
        y_r = point.y + r_y_ * sin(atan(an));
        if ((an > 0 && (y_l >= point.y || y_r <= point.y)) || (an < 0 && (y_r >= point.y || y_l <= point.y)))
            return false;
    }
    else if (turnPosition == 1 && next.y - point.y != 0)
    { // 左转
        x_l = point.x - l_y_ * cos(atan(an));
        y_l = point.y + l_y_ * sin(atan(an));
        x_r = point.x + r_y_ * cos(atan(an));
        y_r = point.y - r_y_ * sin(atan(an));
        if ((an > 0 && (y_l <= point.y || y_r >= point.y)) || (an < 0 && (y_r <= point.y || y_l >= point.y)))
            return false;
    }

    x_lp = cv::Point3f(x_l + x_, y_l, car_height_ * -1);
    x_rp = cv::Point3f(x_r + x_, y_r, car_height_ * -1);
    return true;
}

void savePathplanPoints(const IdpPlanningTrajectory &pathplan, const IdpPlanningTrajectory &lpathplan)
{
    AINFO << "lpathplan.trajectory size:" << lpathplan.trajectory.size()
          << "lpathplan.trajectory size: " << pathplan.trajectory.size();
    XImagePathPlan *planEdit = XImagePathPlan::Instance();
    if (planEdit)
        planEdit->clearPathPoint();

    auto setPoints = [&](const IdpPlanningTrajectory &traj, auto setL, auto setR, int risk_level = -1)
    {
        for (size_t i = 0; i + 1 < traj.trajectory.size(); ++i)
        {
            if (risk_level != -1 && traj.trajectory[i].risk_level != risk_level)
                continue;
            cv::Point3f x_lp, x_rp;
            if (addPathPlanPoint(traj.trajectory[i], traj.trajectory[i + 1], x_lp, x_rp))
            {
                (planEdit->*setL)(x_lp);
                (planEdit->*setR)(x_rp);
            }
        }
    };

    // 遥控预警区域
    if (!lpathplan.trajectory.empty())
    {
        setPoints(lpathplan, &XImagePathPlan::setLeftPathPoint, &XImagePathPlan::setRightPathPoint, 0);
        setPoints(lpathplan, &XImagePathPlan::setWorningPathPointLeft, &XImagePathPlan::setWornintPathPointRight, 2);
        setPoints(lpathplan, &XImagePathPlan::setRunPathPointLeft, &XImagePathPlan::setRunPathPointRight, 1);
    }

    // 循迹轨迹
    if (!pathplan.trajectory.empty())
    {
        int path_type = 0;
        CustomStack::Instance()->GetConfig("XVideoEditor","path_type", path_type);
        double car_height_ = 0.0;
        CustomStack::Instance()->GetConfig("XVideoEditor","car_height_", car_height_);
        if (path_type == 2)
        { // 循迹轨
            setPoints(pathplan, &XImagePathPlan::setPathPlanPointL, &XImagePathPlan::setPathPlanPointR);
        }
        else if (path_type == 1)
        {
            for (const auto &pt : pathplan.trajectory)
            {
                planEdit->setPathPlanPoints(cv::Point3f(pt.x, pt.y, car_height_ * -1));
            }
        }
    }
}

// 测试使用
//  函数：将 YUV 数据追加到文件中
void appendYUVToFile(void *yuvDataAddress, int yuvDataSize, const std::string &fileName)
{
    // 打开文件，以二进制追加模式
    std::ofstream outFile(fileName, std::ios::binary | std::ios::app);
    if (!outFile)
    {
        std::cerr << "无法打开文件用于追加：" << fileName << std::endl;
        return;
    }

    // 将数据追加到文件
    outFile.write(static_cast<char *>(yuvDataAddress), yuvDataSize);

    // 关闭文件
    outFile.close();
}

struct ChannelStatus
{
    int last_index = -1;
    int unchanged_count = 0;
    std::chrono::steady_clock::time_point last_update;
    bool stuck = false;
};

bool ImageIsNull(const std::string &name, const std::vector<std::string> &list)
{
    if (list.empty())
        return false;

    return std::find(list.begin(), list.end(), name) != list.end();
}

// 输出无效数据的name
void getInvalidImageName(const ara::adsfi::IdpSensorImageList &video_list, std::unordered_map<std::string, ChannelStatus> channel_map,
                         std::vector<std::string> &invalid_names)
{
    // 筛选出无数据的相机
    for (const auto &img : video_list.images)
    {
        const std::string &name = img.header.data_name;
        int cur_index = img.shm_buffer_info.current_index;

        auto &st = channel_map[name];
        auto now = std::chrono::steady_clock::now();

        if (st.last_index != cur_index)
        {
            st.last_index = cur_index;
            st.unchanged_count = 0;
            st.stuck = false;
            st.last_update = now;
        }
        else
        {
            st.unchanged_count++;
            // 判断是否长期未变化
            if (st.unchanged_count > MAX_UNCHANGED && !st.stuck)
            {
                st.stuck = true;
                XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_DATA_INVALID);
                AERROR << "摄像头长期无数据报错，摄像头名字=" << name << "，连续帧数" << st.unchanged_count;
            }
            else if (st.unchanged_count > 10 && !st.stuck)
            {
                XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_DATA_INVALID);
                AERROR << "摄像头长期无数据报警，摄像头名字=" << name << "，连续帧数" << st.unchanged_count;
            }
        }
    }

    for (auto &[name, st] : channel_map)
    {
        if (st.stuck)
            invalid_names.push_back(name);
    }

    // 有效数据
    if (invalid_names.empty())
    {
        XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_DATA_INVALID);
    }
}

// 根据显示数据的最大面积设置主相机
void setMaxAreaToMainCamera(const ara::adsfi::IdpSensorImageList &video_list, const std::vector<IdpAppVideoSplitParam> &split_parames)
{
    float max_area = -1.0f;
    std::string max_area_camera_ = "";
    for (const auto &item : split_parames)
    {
        AINFO << "setMaxAreaToMainCamera item.channel_name:" << item.channel_name;
        // 查找同名图像
        auto it = std::find_if(video_list.images.begin(), video_list.images.end(),
                               [&](const ara::adsfi::IdpSensorImage &img)
                               { return img.header.data_name == item.channel_name; });
        if (it == video_list.images.end())
            continue;

        float w = item.view_area_points.right_bottom_point_x - item.view_area_points.left_top_point_x;
        float h = item.view_area_points.right_bottom_point_y - item.view_area_points.left_top_point_y;
        float area = w * h;
        if (area <= 0)
            continue; // 跳过无效区域

        if (area > max_area)
        {
            max_area = area;
            max_area_camera_ = item.channel_name;
        }
    }

    // 最终只保留最大的
    if (max_area_camera_ != "")
    {
        for (int index = 0; index < video_list.images.size(); index++)
        {
            if (video_list.images[index].header.data_name == max_area_camera_)
            {
                CameraShmParam::SetCameraModuleMainParam("mdc_camera", index);
            }
        }
    }
}

// 统计执行周期时间
double getCycleTimeMs()
{
    using namespace std::chrono;
    static steady_clock::time_point last = steady_clock::now();

    auto now = steady_clock::now();
    double diff = duration_cast<microseconds>(now - last).count() / 1000.0; // 转毫秒
    last = now;
    return diff;
}

bool ImageProcessToData(std::vector<SplitImageParame> splitImages,
                        std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> img_process_ptr, int width, int height,
                        void *canvas)
{
    AINFO << "setMaxAreaToMainCamera [ImageProcess] Split count: " << splitImages.size();
    if (splitImages.empty())
    {
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_SPLIT_INVALID);
        return false;
    }

    // 筛选有效输入
    std::vector<SplitImageParame> validImages;
    validImages.reserve(splitImages.size());
    for (auto &p : splitImages)
    {
        int vw = static_cast<int>(p.view_right_bottom_point.x - p.view_left_top_point.x);
        int vh = static_cast<int>(p.view_right_bottom_point.y - p.view_left_top_point.y);
        AINFO << "setMaxAreaToMainCamera channel name" << p.channel_name << ",width=" << p.width
              << ",height=" << p.height << ",vw=" << vw << ",vh=" << vh;
        if (!p.src_ptr || p.width <= 0 || p.height <= 0)
            continue;

        AINFO << "setMaxAreaToMainCamera" << p.channel_name << "   width" << vw << "   height" << vh;
        if (vw <= 0 || vh <= 0)
            continue;

        validImages.push_back(p);
    }

    AINFO << "setMaxAreaToMainCamera validImages size=" << validImages.size();
    if (validImages.empty())
    {
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_SPLIT_INVALID);
        return false;
    }
    XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_SPLIT_INVALID);

    static int index_value = 1;
    std::string idx_str = std::to_string(index_value++);

    AINFO << "setMaxAreaToMainCamera name:" << validImages[0].channel_name << ",width=" << validImages[0].width
          << ",height=" << validImages[0].height;
    // 单张图优化路径
    if (validImages.size() == 1)
    {
        auto &p = validImages[0];
        if (p.index == 0)
        {
            avos::idpware::SImgProcessInfo meminfo;
            meminfo.m_buf_len = width * height * 3 / 2;
            img_process_ptr->idpDvppMalloc(meminfo);
            p.src_ptr = meminfo.m_buf;
        }

        if (p.width >= 1920)
        {
            avos::idpware::SImgProcess resize;
            resize.SetProcessName("resize_" + idx_str);
            resize.InitInputImage(p.src_ptr, p.totalSize, p.width, p.height, avos::idpware::BUF_DVPP_TYPE,
                                  avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            resize.InitOutputImage(canvas, width * height * 3 / 2, width, height, avos::idpware::BUF_DVPP_TYPE,
                                   avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            resize.SetResizeParam(p.width, p.height, width, height);
            img_process_ptr->idpImgProcess(resize);
            return true;
        }
    }

    bool main_done = false;
    for (auto &p : validImages)
    {
        int view_x = static_cast<int>(p.view_left_top_point.x);
        int view_y = static_cast<int>(p.view_left_top_point.y);
        int view_w = static_cast<int>(p.view_right_bottom_point.x - p.view_left_top_point.x);
        int view_h = static_cast<int>(p.view_right_bottom_point.y - p.view_left_top_point.y);
        int crop_x = static_cast<int>(p.crop_left_top_point.x);
        int crop_y = static_cast<int>(p.crop_left_top_point.y);
        int crop_w = static_cast<int>(p.crop_right_bottom_point.x - p.crop_left_top_point.x);
        int crop_h = static_cast<int>(p.crop_right_bottom_point.y - p.crop_left_top_point.y);

        // 初始化内存
        if (p.index == 0)
        {
            avos::idpware::SImgProcessInfo meminfo;
            meminfo.m_buf_len = width * height * 3 / 2;
            img_process_ptr->idpDvppMalloc(meminfo);
            p.src_ptr = static_cast<u_int8_t *>(meminfo.m_buf);
        }

        // 统一封装处理过程
        auto process = [&](const std::string &name, auto setupFunc)
        {
            avos::idpware::SImgProcess param;
            param.SetProcessName(name);
            param.InitInputImage(p.src_ptr, p.totalSize, p.width, p.height, avos::idpware::BUF_DVPP_TYPE,
                                 avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            param.InitOutputImage(canvas, width * height * 3 / 2, width, height, avos::idpware::BUF_DVPP_TYPE,
                                  avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
            setupFunc(param);
            img_process_ptr->idpImgProcess(param);
        };

        if (!main_done)
        {
            main_done = true;
            AINFO << "setMaxAreaToMainCamera chanel:name"
                  << ",width=" << p.width << ",height=" << p.height << ",view_x=" << view_x << ",view_y=" << view_y
                  << ",view_w=" << view_w << ",view_h=" << view_h << ",crop_x=" << crop_x << ",crop_y=" << crop_y
                  << ",crop_w=" << crop_w << ",crop_h=" << crop_h;
            process("img_bottom", [&](auto &p_)
                    { p_.SetResizeParam(p.width, p.height, view_w, view_h); });
            continue;
        }

        // 裁剪 + 粘贴逻辑
        if (crop_w > 0 && crop_h > 0 && crop_x + crop_w <= p.width && crop_y + crop_h <= p.height)
        {
            if (crop_w == view_w && crop_h == view_h)
            {
                process("crop_" + p.channel_name,
                        [&](auto &p_)
                        { p_.SetCropPaste(crop_w, crop_h, crop_x, crop_y, view_x, view_y); });
            }
            else
            {
                process("resize_" + p.channel_name,
                        [&](auto &p_)
                        { p_.SetResizePaste(p.width, p.height, view_w, view_h, view_x, view_y); });
            }
        }
        else
        {
            process("resize_new_" + p.channel_name,
                    [&](auto &p_)
                    { p_.SetResizePaste(p.width, p.height, view_w, view_h, view_x, view_y); });
        }
    }

    return true;
}

void XVideoEditor_Dvpp(const ara::adsfi::IdpSensorImageList &video_list, // 视频组
                       const ara::adsfi::IdpSensorImage &video_data,     // 透明底盘数据
                       const ara::adsfi::IdpAppVideoParameter &parame,   // 切换参数
                       ara::adsfi::IdpSensorImage &image)
{
    timeval tv_start{};
    gettimeofday(&tv_start, nullptr);
    AINFO << "XVideoEditor_Dvpp start time" << tv_start.tv_sec << ":" << std::setprecision(20)
          << tv_start.tv_usec / 1000000.0 << "imagelisttime:" << video_list.header.time;

    static std::string camera_name = "front";
    static std::int32_t index = 0;
    static std::shared_ptr<ZeroCopyShmMem> zero_shm_mem_ptr = nullptr;
    static bool init = false;
    static std::shared_ptr<YUYV420UV12> merge_yuv = nullptr;
    static double width = VIDEO_IMAGE_MAX_WIDTH;
    static double height = VIDEO_IMAGE_MAX_HEIGHT;
    static cv::Ptr<cv::freetype::FreeType2> ft2 = nullptr;
    static void *result_image = nullptr;
    static int frame_num = 0;

    static std::shared_ptr<avos::idpware::SImgProcessInfo> meminfo_front = nullptr;
    static std::shared_ptr<avos::idpware::SImgProcessInfo> meminfo_back = nullptr;

    static unsigned int seq;
    static std::unordered_map<std::string, ChannelStatus> channel_status_map = {};
    std::shared_ptr<avos::idpware::CUserMdc610FuncInterface> img_process_ptr =
        avos::idpware::CUserMdc610FuncInterface::Instance();

    // 执行周期时间
    double cycle_time = getCycleTimeMs();
    if (cycle_time > 50)
    {
        AERROR << "XVideoEditor_Dvpp error exect outtime" << cycle_time;
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_FREQUENCY_ERROR);
    }
    else
    {
        XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_FREQUENCY_ERROR);
    }

    // 检测数据有效性
    if (parame.editor_parameter.split_parames_.size() <= 0)
    {
        AERROR << "XVideoEditor_Dvpp error "
                  "parame.editor_parameter.split_parames_.size()  <= 0";
        auto ptr = CustomStack::Instance();
        // ptr->SetDataValid(&image, false);
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_DTAALIST_EMPTY);
        return;
    }
    else
    {
        XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_DTAALIST_EMPTY);
    }

    const auto &video_list_ = video_list.images;
    if (video_list_.size() <= 0)
    {
        AINFO << "XVideoEditor_Dvpp error video_list_ size is 0, 相机数据无输入";
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_DATA_ERROR);
        // auto ptr = CustomStack::Instance();
        // ptr->SetDataValid(&image, false);
        return;
    }
    else
    {
        XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_DATA_ERROR);
    }

    if (!init)
    {
        zero_shm_mem_ptr = std::make_shared<ZeroCopyShmMem>();
        merge_yuv = YUYV420UV12::white_img(width, height);
        result_image = merge_yuv->get_data();
        init = true;

        // ft2 = cv::freetype::createFreeType2();
        auto ptr = CustomStack::Instance();
        auto fontPath = ptr->GetNodeConfigPath() + "/" + "wqy-microhei.ttc";
        if (!std::filesystem::exists(fontPath))
        {
            XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_PARAMETER_ERROR);
        }
        else
        {
            XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_PARAMETER_ERROR);
        }
        // ft2->loadFontData(fontPath, 0);
    }

    // 如果图像分辨率出现变化，重新初始化
    //  if (width != parame.encoding_parameter.width || height != parame.encoding_parameter.height) {
    //      AINFO << "分辨率变化，重新初始化";
    //      width = parame.encoding_parameter.width;
    //      height = parame.encoding_parameter.height;
    //      merge_yuv = YUYV420UV12::white_img(width, height);
    //      result_image = merge_yuv->get_data();
    //  }

    std::vector<std::string> stuck_channels;
    getInvalidImageName(video_list, channel_status_map, stuck_channels);

    // 设置主相机
    setMaxAreaToMainCamera(video_list, parame.editor_parameter.split_parames_);

    // 添加路径点
    savePathplanPoints(parame.editor_parameter.path, parame.editor_parameter.remote_control);

    // 筛选需要拼接的数据
    std::vector<SplitImageParame> split_images;
    split_images.clear();

    AINFO << "XVideoEditor_Dvpp start parse name";

    for (const auto &item : parame.editor_parameter.split_parames_)
    {
        AINFO << "XVideoEditor_Dvpp item.channel_name:" << item.channel_name;

        if (item.channel_name == "fisheye_stitched")
        {
            AINFO << "fisheye_stitched";
            SplitImageParame image_parame;
            image_parame.src_ptr = (void *)(video_data.data.data());
            if (image_parame.src_ptr == nullptr)
            {
                image_parame.index = 0;
            }
            else
            {
                image_parame.index = 1;
            }

            if (video_data.width <= 0 || video_data.height <= 0)
            {
                AINFO << "XVideoEditor_Dvpp error,size error width=" << video_data.width
                      << ",height=" << video_data.height;
            }

            image_parame.width = video_data.width;
            image_parame.height = video_data.height;
            image_parame.totalSize = video_data.width * video_data.height * 1.5;
            image_parame.channel_name = item.channel_name;
            image_parame.view_left_top_point = cv::Point2f(0, 0);
            image_parame.view_right_bottom_point = cv::Point2f(720, 1080);
            image_parame.crop_left_top_point = cv::Point2f(0, 0);
            image_parame.crop_right_bottom_point = cv::Point2f(width, height);
            split_images.push_back(std::move(image_parame));
        }
        else
        {
            // 查找同名图像
            auto it = std::find_if(video_list.images.begin(), video_list.images.end(),
                                   [&](const ara::adsfi::IdpSensorImage &img)
                                   { return img.header.data_name == item.channel_name; });
            if (it == video_list.images.end())
            {
                AINFO << "XVideoEditor_Dvpp error item.channel_name:" << item.channel_name
                      << " not found in video_list";
                continue;
            }

            SplitImageParame image_parame;

            // 绘制路径（仅前/后相机）
            if (item.channel_name == "front" || item.channel_name == "back")
            {
                std::shared_ptr<avos::idpware::SImgProcessInfo> ptr;
                if ("front" == item.channel_name)
                {
                    if (nullptr == meminfo_front)
                    {
                        meminfo_front = std::make_shared<avos::idpware::SImgProcessInfo>();
                        meminfo_front->m_buf_len = it->width * it->height * 3 / 2;
                        img_process_ptr->idpDvppMalloc(*meminfo_front);
                    }
                    ptr = meminfo_front;
                }
                else
                {
                    if (nullptr == meminfo_back)
                    {
                        meminfo_back = std::make_shared<avos::idpware::SImgProcessInfo>();
                        meminfo_back->m_buf_len = it->width * it->height * 3 / 2;
                        img_process_ptr->idpDvppMalloc(*meminfo_back);
                    }
                    ptr = meminfo_back;
                }
                auto cfg = CustomStack::Instance();
                std::string front_ktdx_, back_ktdx_;
                cfg->GetConfig("XVideoEditor","front_ktdx_", front_ktdx_);
                cfg->GetConfig("XVideoEditor","back_ktdx_", back_ktdx_);
                std::string type = (item.channel_name == "front") ? front_ktdx_ : back_ktdx_;
                auto plan = XImagePathPlan::Instance();

                image_parame.src_ptr = ptr->m_buf;

                // 多做一次转换-----防止在原图上绘制影响到后续使用
                avos::idpware::SImgProcess param;
                param.SetProcessName("copy_image_data");
                param.InitInputImage((void *)(it->data.data()), it->width * it->height * 1.5, it->width, it->height,
                                     avos::idpware::BUF_DVPP_TYPE, avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
                param.InitOutputImage(image_parame.src_ptr, it->width * it->height * 1.5, it->width, it->height,
                                      avos::idpware::BUF_DVPP_TYPE, avos::idpware::IMG_FORMAT_YUV_SEMIPLANAR_420);
                param.SetResizeParam(it->width, it->height, it->width, it->height);
                static int lastRetProcess = 0;
                int retProcess = img_process_ptr->idpImgProcess(param);
                if (lastRetProcess != retProcess)
                {
                    AINFO << "XVideoEditor_Dvpp error lastRet=" << lastRetProcess << ",ret=" << retProcess;
                    lastRetProcess = retProcess;
                }

                plan->drawRemotePathPlan(image_parame.src_ptr, type);
                plan->drawAutoPathPlan(image_parame.src_ptr, type);
                if ("front" == item.channel_name)
                {
                    drawTargetInfo(image_parame.src_ptr, ft2, it->width, it->height, parame.editor_parameter.objects);
                }
            }
            else
            {
                image_parame.src_ptr = (void *)(it->data.data());
            }

            image_parame.index = ImageIsNull(item.channel_name, stuck_channels) ? 0 : 1;

            image_parame.width = it->width;
            image_parame.height = it->height;
            image_parame.totalSize = it->width * it->height * 1.5;
            image_parame.channel_name = item.channel_name;
            image_parame.view_left_top_point =
                cv::Point2f(item.view_area_points.left_top_point_x, item.view_area_points.left_top_point_y);
            image_parame.view_right_bottom_point =
                cv::Point2f(item.view_area_points.right_bottom_point_x, item.view_area_points.right_bottom_point_y);
            image_parame.crop_left_top_point =
                cv::Point2f(item.crop_area_points.left_top_point_x, item.crop_area_points.left_top_point_y);
            image_parame.crop_right_bottom_point =
                cv::Point2f(item.crop_area_points.right_bottom_point_x, item.crop_area_points.right_bottom_point_y);
            split_images.push_back(std::move(image_parame));
        }
    }

    bool reult = ImageProcessToData(split_images, img_process_ptr, width, height, result_image);
    if (split_images.empty())
    {
        AINFO << "XVideoEditor_Dvpp error video is empty";
    }
    if (!reult)
    {
        AINFO << "XVideoEditor_Dvpp error result is false";
        auto ptr = CustomStack::Instance();
        // ptr->SetDataValid(&image, false);
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_DATARESULT_ERROR);
        return;
    }
    else if (result_image == nullptr)
    {
        AINFO << "XVideoEditor_Dvpp error result_image is null";
        auto ptr = CustomStack::Instance();
        // ptr->SetDataValid(&image, false);
        XImagePathPlan::Instance()->ec_add(XImagePathPlan::_ERRORCODE_DATARESULT_ERROR);
        return;
    }
    else
    {
        XImagePathPlan::Instance()->ec_remove(XImagePathPlan::_ERRORCODE_DATARESULT_ERROR);
    }

    //     //处理筛选后的数据
    seq++;

    image.header.seq = seq;
    image.header.time = video_list.header.time;
    image.header.module_name = "yuyv_merge_dvp";
    image.header.data_name = "yuyv_merge_dvp";

    image.height = height;
    image.width = width;
    image.encoding = video_list.images[0].encoding;

    image.shm_buffer_info.shm_key = "yuyv_merge_dvp";
    image.shm_buffer_info.buffer_size = VIDEO_OUPUT_SHM_MAX_WIDTH * VIDEO_OUPUT_SHM_MAX_HEIGHT * 3 / 2;
    image.shm_buffer_info.current_index = seq % 10;
    image.shm_buffer_info.queue_len = 10;

    auto shm_ptr =
        zero_shm_mem_ptr->GetAllocShmMem(image.shm_buffer_info.shm_key, image.shm_buffer_info.buffer_size,
                                         image.shm_buffer_info.current_index, image.shm_buffer_info.queue_len);
    AINFO << "[GetAllocShmMem AA]" << image.shm_buffer_info.shm_key << " " << image.shm_buffer_info.buffer_size;
    if (nullptr == shm_ptr || nullptr == result_image)
    {
        assert(nullptr != shm_ptr);
        assert(nullptr != result_image);
        auto ptr = CustomStack::Instance();
        // ptr->SetDataValid(&image, false);
        AERROR << "XVideoEditor_Dvpp error this is shm_ptr is nullther";
        return;
    }

    
    // VideoEditYUV420UV12::DoYUV420YU12ToImgMode((uint8_t*)result_image, width *
    // height * 3 / 2, width, height,
    //                                            parame.editor_parameter.output_mode);
    memcpy(shm_ptr, result_image, image.width * image.height * 3 / 2);
	if(frame_num == 100)
        appendYUVToFile(result_image,image.width * image.height * 3 / 2,"/opt/usr/img_edit_src.yuv420");
    // result_image = nullptr;
    split_images.clear();

    merge_yuv->resetToWhite();

    timeval tv_end{};
    gettimeofday(&tv_end, nullptr);
    AINFO << "XVideoEditor_Dvpp end time" << tv_end.tv_sec << ":" << std::setprecision(20)
          << tv_end.tv_usec / 1000000.0;
    frame_num++;
}