#ifndef XIMAGEPATHPLAN_H_
#define XIMAGEPATHPLAN_H_
#include <math.h>
#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "EC400.hpp"
#include "StringUtils.hpp"
#include "VideoEditYUV420UV12.hpp"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"

#define MODEL_PINHOLE 0
#define MODEL_FISHEYE 1

#define UP_SPEED (40.0 / 3.6)

#define MAX_PATH_LENGTH 50.0f

enum class VelocityGroup { STOP, BACKWARD, LOW, MID, HIGH };

typedef struct DistortionParams {
    int   camera_model = 0;
    float params_d[5] = {0, 0, 0, 0, 0};
    float epsilon = 0;
} DataType;

// 自主辅助线段存储（支持同一速度组的多个独立段）
struct AutoPathSegment {
    VelocityGroup            velocity_group;
    std::vector<cv::Point3f> left_points;
    std::vector<cv::Point3f> right_points;
};

struct CameraIntrinsics {
    std::string                camera_type_;
    float                      camera_width_ = 0.0;
    float                      camera_height_ = 0.0;
    Eigen::Matrix<float, 3, 3> intr_K_;

    DistortionParams           intr_D_;
    float                      epsilon;
    Eigen::Matrix<float, 3, 3> extr_R_init_;
    Eigen::Matrix<float, 3, 3> extr_R_;
    Eigen::Matrix<float, 3, 1> extr_t_;
};

struct CameraParam {
    int    path_type = -1;       // 0: 前视相机，1：后视相机
    double front_x = 0;          // 相机相对车辆中心 X 偏移
    double back_x = 0;           // 相机相对车辆中心 X 偏移
    double auto_front_x = 0;     // 自主前视相机偏移
    double auto_back_x = 0;      // 自主后视相机偏移
    double front_left = 0;       // 左边界距离
    double front_right = 0;      // 右边界距离
    double back_left = 0;        // 左边界距离
    double back_right = 0;       // 右边界距离
    double front_height = 0;     // 相机高度
    double back_height = 0;      // 相机高度
    double plan_path_length = 0; // 规划路径长度

    CameraParam() {
    }
};

class XImagePathPlan {
private:
    XImagePathPlan() {
        cameras_intrinsics_.clear();
        ApInfo("video_editor") << "XImagePathPlan init";
        std::string prefix;
        std::string front_ktdx_, back_ktdx_;
        auto        front_ktdx_path = prefix + "video/edit/front_ktdx_type";
        auto        ptr = CustomStack::Instance();
        if (!ptr->GetProjectConfigValue(front_ktdx_path, front_ktdx_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, front_ktdx_path);
            throw std::runtime_error("read project config " + front_ktdx_path + " failed");
        }

        auto back_ktdx_path = prefix + "video/edit/back_ktdx_type";
        if (!ptr->GetProjectConfigValue(back_ktdx_path, back_ktdx_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, back_ktdx_path);
            throw std::runtime_error("read project config " + back_ktdx_path + " failed");
        }

        auto alp_plan_path = prefix + "video/edit/path_plan_alpha";
        if (!ptr->GetProjectConfigValue(alp_plan_path, alpha_)) {
            EC400::Instance()->ec_add(EC400::CONFIG, alp_plan_path);
            throw std::runtime_error("read project config " + alp_plan_path + " failed");
        }
        EC400::Instance()->ec_remove(EC400::CONFIG);

        std::string camera_intrinsics_path1 = ptr->GetVehicleConfigPath() + "/cameras_distortion.proj_cfg";

        setCameraDistortion(front_ktdx_);
        setCameraDistortion(back_ktdx_);
    };

public:
    static XImagePathPlan* Instance() {
        static XImagePathPlan* in = nullptr;
        if (in == nullptr) {
            in = new XImagePathPlan();
        }
        return in;
    }

    ~XImagePathPlan(){};

public:
    //获取辅助线
    void setLeftPathPoint(cv::Point3f point);
    void setRightPathPoint(cv::Point3f point);

    void setWorningPathPointLeft(cv::Point3f point);
    void setWornintPathPointRight(cv::Point3f point);

    void setRunPathPointLeft(cv::Point3f point);
    void setRunPathPointRight(cv::Point3f point);

    void setPathPlanPointL(cv::Point3f point);
    void setPathPlanPointR(cv::Point3f point);

    void setPathPlanPoints(cv::Point3f point);

    //添加自主辅助线
    void setAutoPathPlanPoints(VelocityGroup group, const cv::Point3f& pointl, const cv::Point3f& pointr);
    // 添加自主辅助线段（新接口，支持独立段绘制）
    void addAutoPathSegment(VelocityGroup group, const std::vector<cv::Point3f>& left_points,
                            const std::vector<cv::Point3f>& right_points);

    void clearPathPoint();

    void getLeftPathPoint(const std::string& type, std::vector<cv::Point>& points);
    void getRightPathPoint(const std::string& type, std::vector<cv::Point>& points);

    void getWorningPathPointLeft(const std::string& type, std::vector<cv::Point>& points);
    void getWornintPathPointRight(const std::string& type, std::vector<cv::Point>& points);

    void getRunPathPointLeft(const std::string& type, std::vector<cv::Point>& point);
    void getRunPathPointRight(const std::string& type, std::vector<cv::Point>& point);

    void getPathPlanPointL(const std::string& type, std::vector<cv::Point>& points, std::string& color);
    void getPathPlanPointR(const std::string& type, std::vector<cv::Point>& points, std::string& color);

    void drawRemotePathPlan(void* ptr, const std::string& type, const std::string& camera_type);
    void drawAutoPathPlan(void* ptr, const std::string& type, const std::string& camera_type);

protected:
    void getPointByPoint3f(const std::string& camera_type, std::vector<cv::Point>& points,
                           std::vector<cv::Point3f> point_vec, std::string type = "");

    void getPointByPoint3fs(const std::string& camera_type, std::vector<cv::Point>& points_l,
                            std::vector<cv::Point>& points_r, std::vector<cv::Point3f> point_vec_l,
                            std::vector<cv::Point3f> point_vec_r, std::string type = "");

    std::vector<cv::Point> createPolygon(std::vector<cv::Point>& curve1, std::vector<cv::Point>& curve2);

public:
    bool setCameraDistortion(const std::string& type);

    bool ConvertPointVCS2IMG(const std::string& type, const cv::Point3f& p3fVcs, cv::Point3f& p3fImg);

private:
    bool     FileExists(const std::string file_path);
    bool     LoadCameraIntrinsics(const std::string& yaml_file, std::string type = "");
    bool     LoadCameraExtrinsics(const std::string& yaml_file);
    void     PixelDenormalize(const std::string& type, cv::Point3f& pt);
    bool     UndistortPointFisheye(const cv::Point2f& pt, cv::Point2f& upoint);
    void     ProjectInImageFisheye(const std::string& type, cv::Point3f& pt);
    YUVColor getAutoPlanSpeedToColor(VelocityGroup speed);

    /***********************************************前相机参数*******************************/
    float                      camera_width_ = 0.0;
    float                      camera_height_ = 0.0;
    Eigen::Matrix<float, 3, 3> intr_K_;

    DistortionParams           intr_D_;
    float                      epsilon;
    Eigen::Matrix<float, 3, 3> extr_R_init_;
    Eigen::Matrix<float, 3, 3> extr_R_;
    Eigen::Matrix<float, 3, 1> extr_t_;

    /*****************************************应用相机内参和外参*****************************************************/

private:
    std::vector<cv::Point3f> pathPoints_l;
    std::vector<cv::Point3f> pathPoints_r;

    std::vector<cv::Point3f> warningPathPoint_l;
    std::vector<cv::Point3f> warningPathPoint_r;

    std::vector<cv::Point3f> runPathPoint_l;
    std::vector<cv::Point3f> runPathPoint_r;

    std::vector<cv::Point3f> pathplanPoint_l;
    std::vector<cv::Point3f> pathplanPoint_r;

    std::vector<cv::Point3f> pathplan_Points;

    std::map<int, std::vector<cv::Point3f>> auto_pathplanPoint_l;
    std::map<int, std::vector<cv::Point3f>> auto_pathplanPoint_r;

    std::vector<AutoPathSegment> auto_path_segments_;

    cv::Point3f target_Point{0, 0, 0};
    cv::Point3f head_Point{0, 0, 0};

    std::string camera_type = "front_h100";

    int   bottom_length = 0;
    float alpha_ = 0.2;

    std::vector<std::shared_ptr<CameraIntrinsics>> cameras_intrinsics_;
};

#endif // XIMAGEPATHPLAN_H_
