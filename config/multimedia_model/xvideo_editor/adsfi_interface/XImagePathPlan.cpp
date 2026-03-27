#include <math.h>

#include "XImagePathPlan.h"
#include "log/log.h"

void XImagePathPlan::getPointByPoint3f(const std::string& camera_type, std::vector<cv::Point>& points,
                                       std::vector<cv::Point3f> point_vec, std::string type) {
    for (const auto& obj : point_vec) {
        cv::Point3f p3fVcs(obj.x, obj.y, obj.z);
        cv::Point3f p3fImg;
        if (ConvertPointVCS2IMG(camera_type, p3fVcs, p3fImg)) {
            cv::Point pt(p3fImg.x, p3fImg.y);
            points.push_back(pt);
        }
    }
}

// 构建多边形区域，确保曲线不相交且能界定填充区域
std::vector<cv::Point> XImagePathPlan::createPolygon(std::vector<cv::Point>& curve1, std::vector<cv::Point>& curve2) {
    std::vector<cv::Point> polygon(curve1);
    polygon.insert(polygon.end(), curve2.rbegin(), curve2.rend());
    return polygon;
}

void XImagePathPlan::setLeftPathPoint(cv::Point3f point) {
    pathPoints_l.push_back(point);
}

void XImagePathPlan::setRightPathPoint(cv::Point3f point) {
    pathPoints_r.push_back(point);
}

void XImagePathPlan::setWorningPathPointLeft(cv::Point3f point) {
    warningPathPoint_l.push_back(point);
}

void XImagePathPlan::setWornintPathPointRight(cv::Point3f point) {
    warningPathPoint_r.push_back(point);
}

void XImagePathPlan::setRunPathPointLeft(cv::Point3f point) {
    runPathPoint_l.push_back(point);
}

void XImagePathPlan::setRunPathPointRight(cv::Point3f point) {
    runPathPoint_r.push_back(point);
}

void XImagePathPlan::setPathPlanPointL(cv::Point3f point) {
    pathplanPoint_l.push_back(point);
}

void XImagePathPlan::setPathPlanPointR(cv::Point3f point) {
    pathplanPoint_r.push_back(point);
}

void XImagePathPlan::setPathPlanPoints(cv::Point3f point) {
    pathplan_Points.push_back(point);
}

void XImagePathPlan::getLeftPathPoint(const std::string& type, std::vector<cv::Point>& points) {
    // 绘制规划的路径
    getPointByPoint3f(type, points, pathPoints_l, "blue");
}

void XImagePathPlan::getRightPathPoint(const std::string& type, std::vector<cv::Point>& points) {
    getPointByPoint3f(type, points, pathPoints_r, "blue");
}

void XImagePathPlan::getWorningPathPointLeft(const std::string& type, std::vector<cv::Point>& points) {
    getPointByPoint3f(type, points, warningPathPoint_l, "red");
}

void XImagePathPlan::getWornintPathPointRight(const std::string& type, std::vector<cv::Point>& points) {
    getPointByPoint3f(type, points, warningPathPoint_r, "red");
}

void XImagePathPlan::getRunPathPointLeft(const std::string& type, std::vector<cv::Point>& points) {
    //绘制可同行区域
    getPointByPoint3f(type, points, runPathPoint_l, "yellow");
}

void XImagePathPlan::getRunPathPointRight(const std::string& type, std::vector<cv::Point>& points) {
    getPointByPoint3f(type, points, runPathPoint_r, "yellow");
}

void XImagePathPlan::getPathPlanPointL(const std::string& type, std::vector<cv::Point>& points) {
    getPointByPoint3f(type, points, pathplanPoint_l, "green");
}

void XImagePathPlan::getPathPlanPointR(const std::string& type, std::vector<cv::Point>& points) {
    getPointByPoint3f(type, points, pathplanPoint_r, "green");
}

void XImagePathPlan::clearPathPoint() {
    pathPoints_l.clear();
    pathPoints_r.clear();

    warningPathPoint_l.clear();
    warningPathPoint_r.clear();

    runPathPoint_l.clear();
    runPathPoint_r.clear();

    pathplanPoint_l.clear();
    pathplanPoint_r.clear();

    pathplan_Points.clear();
}

/***************************************************以下是相机内参和外参使用接口******************************************************
 */

bool XImagePathPlan::setCameraDistortion(const std::string& type) {
    auto        ptr = CustomStack::Instance();
    std::string camera_intrinsics_path1 = "/opt/usr/zxz/config/cameras_distortion.proj_cfg";
    bool        success1 = LoadCameraIntrinsics(camera_intrinsics_path1, type);
    return success1;
}

bool XImagePathPlan::FileExists(const std::string file_path) {
    if (!std::__fs::filesystem::exists(file_path)) {
        std::cout << "yaml file not found: " << file_path;
        return false;
    }

    std::ifstream ifs(file_path);
    if (!ifs.is_open()) {
        AINFO << "radar2cam_fusion: opening failed. check if " << file_path << " exists";
        return -1;
    } else {
        AINFO << "radar2cam_fusion: parsing yaml " << file_path << std::flush;
    }
    ifs.close();
    return true;
}

bool XImagePathPlan::LoadCameraIntrinsics(const std::string& yaml_file, std::string type) {
    if (!FileExists(yaml_file)) {
        ec_add(_ERRORCODE_PARAMETER_ERROR);
        return false;
    }

    try {
        YAML::Node node = YAML::LoadFile(yaml_file);
        if (node.IsNull()) {
            AINFO << "Load " << yaml_file << " failed! please check!";
            ec_add(_ERRORCODE_PARAMETER_ERROR);
            return false;
        }

        std::vector<std::string> camera_names = node["camera_names"].as<std::vector<std::string>>();
        AINFO << "Camera Names: ";
        auto cameras_intrinsics_info = std::make_shared<CameraIntrinsics>();
        for (const auto& name : camera_names) {
            AINFO << "*************camera_names: " << name << " ";
        }

        // 读取front_h125相机的配置 type=front_h125
        YAML::Node front_h125;
        if (node[type]) {
            front_h125 = node[type];
        } else {
            AERROR << "load camera_names is falied:" << type;
            ec_add(_ERRORCODE_PARAMETER_ERROR);
            return false;
        }

        auto it = std::find_if(
            cameras_intrinsics_.begin(), cameras_intrinsics_.end(),
            [&type](const std::shared_ptr<CameraIntrinsics>& cam) { return cam && cam->camera_type_ == type; });

        if (it != cameras_intrinsics_.end()) {
            // 已存在
            std::cout << "Camera type '" << type << "' already exists.\n";
            return true;
        } else {
            // 不存在，可以插入
            std::cout << "Camera type '" << type << "' not found.\n";
            cameras_intrinsics_info->camera_type_ = type;
        }

        // 读取相机Size
        std::vector<int> size = front_h125["Size"].as<std::vector<int>>();
        if (size.size() == 2) {
            camera_width_ = size[0];
            camera_height_ = size[1];

            cameras_intrinsics_info->camera_width_ = size[0];
            cameras_intrinsics_info->camera_height_ = size[1];
        }

        // 读取相机K内参矩阵
        Eigen::VectorXf    params(9 + 5);
        std::vector<float> K = front_h125["K"].as<std::vector<float>>();
        if (K.size() < 9) {
            ec_add(_ERRORCODE_PARAMETER_ERROR);
            return false;
        } else {
            for (size_t i = 0; i < K.size(); i++) {
                intr_K_(i / 3, i % 3) = K[i];
            }
            cameras_intrinsics_info->intr_K_ = intr_K_;
        }
        if (front_h125["Epsilon"]) {
            intr_D_.camera_model = MODEL_FISHEYE;
            intr_D_.epsilon = front_h125["Epsilon"].as<float>();
            Eigen::VectorXf params_d(4);
            for (size_t i = 0; i < 4; ++i) {
                intr_D_.params_d[i] = front_h125["D"][i].as<float>();
            }
            cameras_intrinsics_info->intr_D_ = intr_D_;
        } else {
            intr_D_.camera_model = MODEL_PINHOLE;
            Eigen::VectorXf params_d(5);
            for (size_t i = 0; i < 5; ++i) {
                intr_D_.params_d[i] = front_h125["D"][i].as<float>();
            }
            cameras_intrinsics_info->intr_D_ = intr_D_;
        }

        Eigen::VectorXf params_t(3), params_euler(3);
        try {
            for (int i = 0; i < 3; ++i) {
                params_t(i) = front_h125["t"][i].as<float>();
                params_euler(i) = front_h125["R_euler"][i].as<float>();
            }
            params_euler = params_euler / 180.0 * M_PI;
        } catch (const YAML::Exception& e) {
            AINFO << "load camera extrinsics with error,"
                  << " YAML exception: ";
            ec_add(_ERRORCODE_PARAMETER_ERROR);
            return false;
        }

        // fill rotation and translation
        // 初始旋转矩阵，需要根据相机进行调整？
        float roll = 90 * M_PI / 180;   // 3-x-roll
        float pitch = -90 * M_PI / 180; // 2-y-pitch
        float yaw = 0 * M_PI / 180;     // 1-z-yaw
        extr_R_init_ = Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ()) *
                       Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitY()) *
                       Eigen::AngleAxisf(roll, Eigen::Vector3f::UnitX());
        cameras_intrinsics_info->extr_R_init_ = extr_R_init_;

        Eigen::Quaternionf rotation = (Eigen::AngleAxisf(params_euler.z(), // pitch
                                                         Eigen::Vector3f::UnitZ()) *
                                       Eigen::AngleAxisf(params_euler.y(), Eigen::Vector3f::UnitY()) *
                                       Eigen::AngleAxisf(params_euler.x(), Eigen::Vector3f::UnitX()));
        extr_R_ = rotation.matrix();
        cameras_intrinsics_info->extr_R_ = extr_R_;

        extr_t_.col(0) << params_t(0), params_t(1), params_t(2);
        cameras_intrinsics_info->extr_t_ = extr_t_;

        cameras_intrinsics_.push_back(cameras_intrinsics_info);
    } catch (const YAML::BadFile& e) {
        AERROR << "YAML file not found or cannot be opened: " << e.what();
        ec_add(_ERRORCODE_PARAMETER_ERROR);
        return false;
    } catch (const YAML::ParserException& e) {
        AERROR << "YAML parse error: " << e.what();
        ec_add(_ERRORCODE_PARAMETER_ERROR);
        return false;
    } catch (const YAML::Exception& e) {
        AERROR << "YAML error: " << e.what();
        ec_add(_ERRORCODE_PARAMETER_ERROR);
        return false;
    }

    ec_remove(_ERRORCODE_PARAMETER_ERROR);
    return true;
}

bool XImagePathPlan::LoadCameraExtrinsics(const std::string& yaml_file) {
    return true;
}

bool XImagePathPlan::ConvertPointVCS2IMG(const std::string& type, const cv::Point3f& p3fVcs, cv::Point3f& p3fImg) {
    auto it = std::find_if(
        cameras_intrinsics_.begin(), cameras_intrinsics_.end(),
        [&type](const std::shared_ptr<CameraIntrinsics>& cam) { return cam && cam->camera_type_ == type; });

    if (it == cameras_intrinsics_.end()) {
        return false;
    }

    auto cam_ptr = *it;

    Eigen::Matrix<float, 3, 1> point_vcs;
    Eigen::Matrix<float, 3, 1> point_camera;
    point_vcs << p3fVcs.x, p3fVcs.y, p3fVcs.z;
    point_camera = cam_ptr->extr_R_ * (cam_ptr->extr_R_init_ * point_vcs + cam_ptr->extr_t_);
    if (point_camera[2] <= 0) {
        return false;
    }
    float x = point_camera[0] / point_camera[2];
    float y = point_camera[1] / point_camera[2];
    float depth = point_camera[2];
    p3fImg = cv::Point3f(x, y, depth);

    if (intr_D_.camera_model == MODEL_PINHOLE) {
        PixelDenormalize(type, p3fImg);
    } else if (intr_D_.camera_model == MODEL_FISHEYE) {
        ProjectInImageFisheye(type, p3fImg);
    }

    double bottom_length_ = 0;
    if (p3fVcs.x >= 0) {
        bottom_length_ = bottom_length;
    }

    if (p3fImg.x >= 0 && p3fImg.x < cam_ptr->camera_width_ && p3fImg.y >= 0 &&
        p3fImg.y < cam_ptr->camera_height_ - bottom_length_ && p3fImg.y > cam_ptr->camera_height_ / 2 + 10) {
        return true;
    } else {
        if (p3fImg.x < 0) {
            p3fImg.x = 0;
        }
        if (p3fImg.x >= cam_ptr->camera_width_) {
            p3fImg.x = cam_ptr->camera_width_ - 1;
        }
        if (p3fImg.y < 0) {
            p3fImg.y = 0;
        }
        if (p3fImg.y >= cam_ptr->camera_height_) {
            p3fImg.y = cam_ptr->camera_height_ - 1;
        }
    }
    return false;
}

void XImagePathPlan::PixelDenormalize(const std::string& type, cv::Point3f& pt) {
    // add distortion
    auto it = std::find_if(
        cameras_intrinsics_.begin(), cameras_intrinsics_.end(),
        [&type](const std::shared_ptr<CameraIntrinsics>& cam) { return cam && cam->camera_type_ == type; });

    if (it == cameras_intrinsics_.end()) {
        return;
    }

    auto cam_ptr = *it;

    Eigen::Matrix<double, 2, 1> pt2d(pt.x, pt.y);
    double                      r_sq = pt2d[0] * pt2d[0] + pt2d[1] * pt2d[1];

    auto params_d = cam_ptr->intr_D_.params_d;

    Eigen::Matrix<double, 2, 1> pt2d_radial =
        pt2d * (1 + params_d[0] * r_sq + params_d[1] * r_sq * r_sq + params_d[4] * r_sq * r_sq * r_sq);
    Eigen::Matrix<double, 2, 1> dpt2d;
    dpt2d[0] = 2 * params_d[2] * pt2d[0] * pt2d[1] + params_d[3] * (r_sq + 2 * pt2d[0] * pt2d[0]);
    dpt2d[1] = params_d[2] * (r_sq + 2 * pt2d[1] * pt2d[1]) + 2 * params_d[3] * pt2d[0] * pt2d[1];

    Eigen::Matrix<double, 2, 1> pt2d_distort;
    pt2d_distort[0] = pt2d_radial[0] + dpt2d[0];
    pt2d_distort[1] = pt2d_radial[1] + dpt2d[1];

    // add intrinsic K
    double focal_length_x = cam_ptr->intr_K_(0, 0);
    double focal_length_y = cam_ptr->intr_K_(1, 1);
    double center_x = cam_ptr->intr_K_(0, 2);
    double center_y = cam_ptr->intr_K_(1, 2);

    pt.x = pt2d_distort[0] * focal_length_x + center_x;
    pt.y = pt2d_distort[1] * focal_length_y + center_y;
}

void XImagePathPlan::ProjectInImageFisheye(const std::string& type, cv::Point3f& pt) {
    auto it = std::find_if(
        cameras_intrinsics_.begin(), cameras_intrinsics_.end(),
        [&type](const std::shared_ptr<CameraIntrinsics>& cam) { return cam && cam->camera_type_ == type; });

    if (it == cameras_intrinsics_.end()) {
        return;
    }

    auto cam_ptr = *it;

    Eigen::Vector3f norm_dir(pt.x, pt.y, 1.0);
    norm_dir.normalize();

    auto params_d = cam_ptr->intr_D_.params_d;
    auto epsilon = cam_ptr->intr_D_.epsilon;

    // project to image plane
    float xu = norm_dir(0) / (norm_dir(2) + epsilon);
    float yu = norm_dir(1) / (norm_dir(2) + epsilon);
    // add distortion
    float r2 = xu * xu + yu * yu;
    float r4 = r2 * r2;

    Eigen::Vector2f pt2d_img;
    pt2d_img[0] =
        (1 + params_d[0] * r2 + params_d[1] * r4) * xu + 2 * params_d[2] * xu * yu + params_d[3] * (r2 + 2 * xu * xu);
    pt2d_img[1] =
        (1 + params_d[0] * r2 + params_d[1] * r4) * yu + params_d[2] * (r2 + 2 * yu * yu) + 2 * params_d[3] * xu * yu;

    // to image pixel
    // transform to image coordinates
    const float fx = cam_ptr->intr_K_(0, 0);
    const float fy = cam_ptr->intr_K_(1, 1);
    const float cx = cam_ptr->intr_K_(0, 2);
    const float cy = cam_ptr->intr_K_(1, 2);
    const float s = cam_ptr->intr_K_(0, 1);
    pt.x = fx * pt2d_img[0] + s * pt2d_img[1] + cx;
    pt.y = fy * pt2d_img[1] + cy;
}

bool XImagePathPlan::UndistortPointFisheye(const cv::Point2f& pt, cv::Point2f& upoint) {
    Eigen::Vector2f pp((pt.x * intr_K_(1, 1) - intr_K_(0, 2) * intr_K_(1, 1) - intr_K_(0, 1) * (pt.y - intr_K_(1, 2))) /
                           (intr_K_(0, 0) * intr_K_(1, 1)),
                       (pt.y - intr_K_(1, 2)) / intr_K_(1, 1)); // plane
    Eigen::Vector2f pu = pp;
    for (int j = 0; j < 4; j++) {
        float r2 = pu[0] * pu[0] + pu[1] * pu[1];
        float r4 = r2 * r2;
        pu[0] = (pp[0] - 2 * intr_D_.params_d[2] * pu[0] * pu[1] - intr_D_.params_d[3] * (r2 + 2 * pu[0] * pu[0])) /
                (1 + intr_D_.params_d[0] * r2 + intr_D_.params_d[1] * r4);
        pu[1] = (pp[1] - 2 * intr_D_.params_d[3] * pu[0] * pu[1] - intr_D_.params_d[2] * (r2 + 2 * pu[1] * pu[1])) /
                (1 + intr_D_.params_d[0] * r2 + intr_D_.params_d[1] * r4);
    }

    auto epsilon = intr_D_.epsilon;
    // project to unit sphere
    float r2 = pu[0] * pu[0] + pu[1] * pu[1];
    float a = r2 + 1;
    float b = 2 * epsilon * r2;
    float c = r2 * epsilon * epsilon - 1;

    if (b * b - 4 * a * c < 0) {
        upoint.x = 1e5;
        upoint.y = 1e5;
        return false;
    }

    float           Zs = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    Eigen::Vector3f Xw(pu[0] * (Zs + epsilon), pu[1] * (Zs + epsilon), Zs);

    if (Xw[2] < 0) {
        upoint.x = 1e5;
        upoint.y = 1e5;
        return false;
    }
    upoint.x = Xw[0] / Xw[2] * intr_K_(0, 0) + intr_K_(0, 2);
    upoint.y = Xw[1] / Xw[2] * intr_K_(1, 1) + intr_K_(1, 2);
    return true;
}

void XImagePathPlan::drawRemotePathPlan(void* ptr, const std::string& type) {
    // if(type == "") return;

    // if (camera_type != type) {
    //     AINFO << "************************************RYF->drawRemotePathPlan" << "camera_type:" << type;
    //     bool success = setCameraDistortion(type);
    //     if (!success) {
    //         AINFO << "setCameraDistortion failed";
    //          ec_add(_ERRORCODE_PARAMETER_ERROR);
    //         return;
    //     } else {
    //         ec_remove(_ERRORCODE_PARAMETER_ERROR);
    //         camera_type = type;
    //     }
    // }

    auto it = std::find_if(
        cameras_intrinsics_.begin(), cameras_intrinsics_.end(),
        [&type](const std::shared_ptr<CameraIntrinsics>& cam) { return cam && cam->camera_type_ == type; });

    if (it == cameras_intrinsics_.end()) {
        AINFO << "************************************RYF->drawRemotePathPlan"
              << "camera_type:" << type;
        bool success = setCameraDistortion(type);
        if (!success) {
            AINFO << "setCameraDistortion failed";
            ec_add(_ERRORCODE_PARAMETER_ERROR);
            return;
        } else {
            ec_remove(_ERRORCODE_PARAMETER_ERROR);
            camera_type = type;
        }
    } else {
        ec_remove(_ERRORCODE_PARAMETER_ERROR);
    }

    auto cam_ptr = *it;

    std::vector<cv::Point> blue_left_points;
    getLeftPathPoint(type, blue_left_points);

    std::vector<cv::Point> blue_right_points;
    getRightPathPoint(type, blue_right_points);

    std::vector<cv::Point> warn_left_points;
    getWorningPathPointLeft(type, warn_left_points);

    std::vector<cv::Point> warn_right_points;
    getWornintPathPointRight(type, warn_right_points);

    std::vector<cv::Point> run_left_points;
    getRunPathPointLeft(type, run_left_points);

    std::vector<cv::Point> run_right_points;
    getRunPathPointRight(type, run_right_points);

    AINFO << "*************************787878787878787warn_right_points：" << warn_right_points.size();

    VideoEditYUV420UV12::drawPathplan(ptr, cam_ptr->camera_width_, cam_ptr->camera_height_, "blue", blue_left_points,
                                      blue_right_points);
    VideoEditYUV420UV12::drawPathplan(ptr, cam_ptr->camera_width_, cam_ptr->camera_height_, "red", warn_left_points,
                                      warn_right_points);
    VideoEditYUV420UV12::drawPathplan(ptr, cam_ptr->camera_width_, cam_ptr->camera_height_, "yellow", run_left_points,
                                      run_right_points);
}

void XImagePathPlan::drawAutoPathPlan(void* ptr, const std::string& type) {
    if (type == "")
        return;

    auto it = std::find_if(
        cameras_intrinsics_.begin(), cameras_intrinsics_.end(),
        [&type](const std::shared_ptr<CameraIntrinsics>& cam) { return cam && cam->camera_type_ == type; });

    if (it == cameras_intrinsics_.end()) {
        bool success = setCameraDistortion(type);
        if (!success) {
            AINFO << "setCameraDistortion failed";
            ec_add(_ERRORCODE_PARAMETER_ERROR);
            return;
        } else {
            ec_remove(_ERRORCODE_PARAMETER_ERROR);
            camera_type = type;
        }
    } else {
        ec_remove(_ERRORCODE_PARAMETER_ERROR);
    }

    auto cam_ptr = *it;

    std::vector<cv::Point> blue_left_points;
    getPathPlanPointL(type, blue_left_points);

    std::vector<cv::Point> blue_right_points;
    getPathPlanPointR(type, blue_right_points);

    VideoEditYUV420UV12::drawPathplan(ptr, cam_ptr->camera_width_, cam_ptr->camera_height_, "green", blue_left_points,
                                      blue_right_points);
}
