#include "perception_fusion_common/common/math_util.h"
#include "math.h"

#include <random>
#include <vector>

namespace avos {
namespace perception {
namespace algorithm {

double GetRange(const double& x,
                const double& y) {
  return (sqrt(pow(x, 2) + pow(y, 2)));
}


double GetDistance(const double &x1,
                            const double &y1,
                            const double &x2,
                            const double &y2) {
  return (sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
}

double GetDistance(const double &x1,
                            const double &y1,
                            const double &z1,
                            const double &x2,
                            const double &y2,
                            const double &z2) {
  return (sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)+pow(z1 - z2, 2)));
}
double GetEuclideanDistance(const double &x1,
                            const double &y1,
                            const double &x2,
                            const double &y2) {
  return (sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
}

double GetCosineDistance(const double &x1,
                         const double &y1,
                         const double &x2,
                         const double &y2) {
  // 余弦相似度的取值范围为[−1,1]。
  // 余弦越大表示两个向量的夹角越小，余弦越小表示两向量的夹角越大
  // 当两个向量的方向重合时余弦取最大值 1, 当两个向量的方向完全相反余弦取最小值-1。
  double v_sum = x1 * x2 + y1 * y2;
  double v_sqrt = sqrt(pow(x1, 2) + pow(y1, 2)) * sqrt(pow(x2, 2) + pow(y2, 2));
  if (v_sqrt == 0) {
    return 1.0;
  }
  return (v_sum / v_sqrt);
}

float GetSpeed(const float &vx,
               const float &vy) {
  return (sqrt(pow(vx, 2) + pow(vy, 2)));
}

void GetLatLonVelocity(const float &vx,
                       const float &vy,
                       const float &heading_deg,
                       float &v_lat,
                       float &v_lon) {
  float theta = heading_deg  / 180.0 * M_PI;
  v_lat = -vx * sin(theta) + vy * cos(theta); // 垂直航向的分速度
  v_lon = vx * cos(theta) + vy * sin(theta); // 沿着航向的分速度
}

// normalize heading to [-180deg, 180deg]
float NormalizeHeading(const float &in_deg) {
  auto out_deg = fmod(in_deg, 360.0);
  if (out_deg < -180.0) { out_deg += 360.0; }
  if (out_deg > 180.0) { out_deg -= 360.0; }
  return out_deg;
}

float GetMeanHeading(const std::deque<float> &buffer) {
  if (buffer.size() == 0) {return 0;}
  // heading mean
  float sum_sin_angle = 0;
  float sum_cos_angle = 0;
  for (const auto& angle : buffer) {
    sum_sin_angle += sin(angle / 180.0 * M_PI);
    sum_cos_angle += cos(angle / 180.0 * M_PI);
  }
  float mean_sin_angle = sum_sin_angle / buffer.size();
  float mean_cos_angle = sum_cos_angle / buffer.size();
  float mean = atan2(mean_sin_angle, mean_cos_angle) / M_PI * 180.0;
  return mean;
}

float GetMean(const std::deque<float> &buffer) {
  if (buffer.size() == 0) {return 0;}
  float sum = std::accumulate(std::begin(buffer), std::end(buffer), 0.0);
  float mean = sum / buffer.size();
  return mean;
}

float GetRMS(const std::deque<float> &buffer) {
  if (buffer.size() == 0) {return 0;}
  float sum = std::accumulate(std::begin(buffer), std::end(buffer), 0.0);
  float mean = sum / buffer.size();
  float accum = 0.0;
  std::for_each(std::begin(buffer), std::end(buffer), [&](const float d) {
    accum += (d - mean) * (d - mean);
  });
  float rms = sqrt(accum / buffer.size());
  return rms;
}

void GetMeanAndRMS(const std::deque<float> &buffer,
                   float &mean,
                   float &rms) {
  if (buffer.size() == 0) {return;}
  float sum = std::accumulate(std::begin(buffer), std::end(buffer), 0.0);
  mean = sum / buffer.size();
  float accum = 0.0;
  std::for_each(std::begin(buffer), std::end(buffer), [&](const float d) {
    accum += (d - mean) * (d - mean);
  });
  rms = sqrt(accum / buffer.size());
}



// heading融合工具，为避免在跨越±180度处出问题，先把角度转换成向量，对向量融合完再转换回角度
cv::Vec2f angle2vec(float angle) {
  return cv::Vec2f(-sin(angle*CV_PI/180),cos(angle*CV_PI/180));
}

cv::Vec2f vecplus(cv::Vec2f vec1, cv::Vec2f vec2) {
  return (vec1+vec2) / norm(vec1+vec2);
}

float vec2angle(cv::Vec2f vec) {
  return -atan2(vec[0], vec[1])*180/CV_PI;
}

float vecdelta(cv::Vec2f vec1, cv::Vec2f vec2) {
  double t = vec1.dot(vec2)/norm(vec1)/norm(vec2);
  if(t >= 1) return 0;
  return acos(t)*180/CV_PI;
}


bool projectPointOnLineSegment(cv::Point2f point,cv::Point2f projection_point,double &dist, cv::Point2f line_start, cv::Point2f line_end) {
    // 计算线段的向量
    cv::Point2f line_vec = line_end - line_start;

    // 计算点到线段起点的向量
    cv::Point2f point_vec = point - line_start;

    // 计算点到线段起点的投影点
    double projection_len = point_vec.dot(line_vec) / line_vec.dot(line_vec);
    projection_point = line_start + projection_len * line_vec;
    dist = norm(projection_point - point);
    //std::cout<<"dist vec len "<<projection_len<<","<<norm(projection_point - point)<<std::endl;

    //return projection_point;
    // 如果投影点在线段上，则返回投影点，否则返回离线段端点最近的点
    if (projection_len >= 0 && projection_len <= 1) {
        return true;
    } 
    else 
    {
      return false;
    }
}

bool judgeinrect(std::vector<cv::Point2f> pts, cv::Point2f point)
{
  //判断点是否在四边形内
  int size_pts = pts.size();
  int zeroplus = 0;
  int zerominus = 0;
  int zeroequal = 0;
  for(int i = 0; i < size_pts; ++i)
  {
    int l = (i+1) % size_pts;
    double re = (pts[l].x - pts[i].x) *(point.y - pts[i].y) - (pts[l].y - pts[i].y) *(point.x - pts[i].x);
    if(re > 0)
      zeroplus++;
    else if(re < 0)
      zerominus++;
    else
      zeroequal++;
  }
  if(zeroplus == size_pts|| zerominus == size_pts || zeroequal>0)
    return true;
  else
    return false;


}

// line_index ;左 上 右 下
int distanceToRotatedRectEdges(cv::Point2f point, cv::RotatedRect rotated_rect) {
    std::vector<cv::Point2f> rect_points(4);
    rotated_rect.points(rect_points.data());

    double min_distance = DBL_MAX;
    int line_index = -1;
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        cv::Point2f _pt;
        double distance = 0;
        if(projectPointOnLineSegment(point,_pt,distance,rect_points[i], rect_points[j]))
        {
          if (distance < min_distance) {
            min_distance = distance;
            line_index = i;
          }
        }
        
    }

    return line_index;;
}
bool fitLine(std::vector<cv::Point2f>& points, float& slope,float& intercept) {
    cv::Mat X(points.size(), 2, CV_32F);
    cv::Mat Y(points.size(), 1, CV_32F);
    // 构建矩阵X和向量Y
    for (int i = 0; i < points.size(); i++) {
        X.at<float>(i, 0) = points[i].x;
        X.at<float>(i, 1) = 1;
        Y.at<float>(i, 0) = points[i].y;
    }

    // 使用最小二乘法进行直线拟合
    cv::Mat coeff;
    int res = cv::solve(X, Y, coeff, cv::DECOMP_QR);
 
    // 提取斜率和截距

    slope = coeff.at<float>(0, 0);
   
    intercept = coeff.at<float>(1, 0);
     if(res==0)
      return false;
    else 
      return true;
}

// RANSAC平面拟合函数
bool ransacPlaneFitting(const std::vector<cv::Point3f>& points, 
                       cv::Vec4f& plane_coeffs, 
                       std::vector<int>& inliers,
                       int iterations , 
                       float threshold) {
  if (points.size() < 3) return false;
  std::cout<<"ransacPlaneFitting iterations:"<<iterations<<" threshold:"<<threshold<<std::endl;
  // 调试：输出点云范围
  cv::Point3f min_pt = points[0], max_pt = points[0];
  for (const auto& p : points) {
    min_pt.x = std::min(min_pt.x, p.x);
    min_pt.y = std::min(min_pt.y, p.y);
    min_pt.z = std::min(min_pt.z, p.z);
    max_pt.x = std::max(max_pt.x, p.x);
    max_pt.y = std::max(max_pt.y, p.y);
    max_pt.z = std::max(max_pt.z, p.z);
  }
  std::cout << "[ransacPlaneFitting] Point cloud range - X: [" << min_pt.x << ", " << max_pt.x 
            << "], Y: [" << min_pt.y << ", " << max_pt.y 
            << "], Z: [" << min_pt.z << ", " << max_pt.z << "]" << std::endl;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, points.size() - 1);
  
  int best_inliers_count = 0;
  cv::Vec4f best_plane;
  std::vector<int> best_inliers;
  
  for (int iter = 0; iter < iterations; ++iter) {
    // 随机选择三个点
    int idx1 = dis(gen);
    int idx2 = dis(gen);
    int idx3 = dis(gen);
    
    // 确保三个点不共线
    cv::Point3f v1 = points[idx2] - points[idx1];
    cv::Point3f v2 = points[idx3] - points[idx1];
    cv::Point3f cross = v1.cross(v2);
    float cross_norm = cv::norm(cross);
    if (cv::norm(cross) < 1e-6) continue; // 三点共线，跳过
    
    // 计算平面方程
    cv::Point3f normal = cross / cross_norm;
    float d = -normal.dot(points[idx1]);
    // 归一化平面方程，避免数值问题
    float norm_factor = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (norm_factor < 1e-6) continue;
    
    cv::Vec4f plane(normal.x / norm_factor, normal.y / norm_factor, normal.z / norm_factor, d / norm_factor);
    
    // 计算内点
    std::vector<int> current_inliers;
    current_inliers.reserve(points.size());
    
    for (size_t j = 0; j < points.size(); ++j) {
      float distance = std::abs(plane[0] * points[j].x + 
                               plane[1] * points[j].y + 
                               plane[2] * points[j].z + plane[3]);
      if (distance < threshold) {
        current_inliers.push_back(j);
      }
    }
    
    // 更新最佳平面
    if (current_inliers.size() > best_inliers_count) {
      best_inliers_count = current_inliers.size();
      best_plane = plane;
      best_inliers = current_inliers;
      
      if (iter % 1 == 0) { // 每100次迭代输出一次进度
        std::cout << "[ransacPlaneFitting] Iteration " << iter << ": found " << best_inliers_count 
                  << " inliers" << std::endl;
      }
    }
  
  }
  
  if (best_inliers_count < 3) {
    std::cout << "[ERROR] RANSAC failed: only " << best_inliers_count 
              << " inliers found" << std::endl;
    return false;
  }
  std::cout << "[ransacPlaneFitting] RANSAC found " << best_inliers_count << " inliers" << std::endl;
  // 构建矩阵 A = [x y z 1]
  cv::Mat A(best_inliers.size(), 4, CV_32F);
  for (size_t i = 0; i < best_inliers.size(); ++i) {
    const cv::Point3f& p = points[best_inliers[i]];
    A.at<float>(i, 0) = p.x;
    A.at<float>(i, 1) = p.y;
    A.at<float>(i, 2) = p.z;
    A.at<float>(i, 3) = 1.0f;
  }
  
  // 使用SVD求解最小二乘问题：min ||A * x||^2, s.t. ||x|| = 1
  cv::Mat w, u, vt;
  cv::SVD::compute(A, w, u, vt, cv::SVD::MODIFY_A | cv::SVD::FULL_UV);
  
  // 解是V的最后一列（最小奇异值对应的特征向量）
  cv::Mat solution = vt.row(vt.rows - 1);
  
  // 提取平面参数
  float a = solution.at<float>(0);
  float b = solution.at<float>(1);
  float c = solution.at<float>(2);
  float d = solution.at<float>(3);
  
  // 归一化平面方程
  float norm_abc = std::sqrt(a * a + b * b + c * c);
  if (norm_abc < 1e-6) {
    std::cout << "[ERROR] Degenerate plane solution" << std::endl;
    return false;
  }
  
  a /= norm_abc;
  b /= norm_abc;
  c /= norm_abc;
  d /= norm_abc;
  
  plane_coeffs = cv::Vec4f(a, b, c, d);
  inliers = best_inliers;
  
  // 验证平面方程
  float avg_z = 0;
  for (const auto& idx : best_inliers) {
    avg_z += points[idx].z;
  }
  avg_z /= best_inliers.size();
  
  std::cout << "[ransacPlaneFitting] Final plane equation: " << a << "x + " << b << "y + " << c << "z + " << d << " = 0" << std::endl;
  std::cout << "[ransacPlaneFitting] Average Z of inliers: " << avg_z << std::endl;
  std::cout << "[ransacPlaneFitting] Inliers count: " << best_inliers.size() << std::endl;
  
  // 检查平面方程的合理性
  if (std::abs(a) > 1e6 || std::abs(b) > 1e6 || std::abs(c) > 1e6 || std::abs(d) > 1e6) {
    std::cout << "[ERROR] Plane coefficients too large - numerical instability detected!" << std::endl;
    std::cout << "[ERROR] a: " << a << ", b: " << b << ", c: " << c << ", d: " << d << std::endl;
    return false;
  }
  
  return true;
}

std::vector<double> getBoundingBox(std::vector<double> boxA, std::vector<double> boxB) { 
    return { std::min(boxA[0], boxB[0]), std::min(boxA[1], boxB[1]), std::max(boxA[2], boxB[2]), std::max(boxA[3], boxB[3]) }; 
}
double area(std::vector<double> box) { 
    return (box[2] - box[0]) * (box[3] - box[1]); 
}
std::vector<double> intersection(std::vector<double> boxA, std::vector<double> boxB) { 
    double x1 = std::max(boxA[0], boxB[0]); 
    double y1 = std::max(boxA[1], boxB[1]); 
    double x2 = std::min(boxA[2], boxB[2]); 
    double y2 = std::min(boxA[3], boxB[3]); 
    return (x2 > x1 && y2 > y1) ? std::vector<double>{x1, y1, x2, y2} : std::vector<double>{0, 0, 0, 0}; 
}
bool GIOU(cv::Rect2f rect1,cv::Rect2f rect2,double &giou) { 

    std::vector<double> boxA,boxB;
    boxA.emplace_back(rect1.tl().x);
    boxA.emplace_back(rect1.tl().y);
    boxA.emplace_back(rect1.br().x);
    boxA.emplace_back(rect1.br().y);
    boxB.emplace_back(rect2.tl().x);
    boxB.emplace_back(rect2.tl().y);
    boxB.emplace_back(rect2.br().x);
    boxB.emplace_back(rect2.br().y);
    std::vector<double> boxI = intersection(boxA, boxB); 
    double areaI = area(boxI); 
    double areaA = area(boxA); 
    double areaB = area(boxB); 
    double areaC = area(getBoundingBox(boxA, boxB)); 
    double U = areaA + areaB - areaI; 
    if(U==0||areaC==0)return false;
    giou =  areaI / U - (areaC - U) / areaC; 
    return std::isfinite(giou);
}



}  // namespace algorithm
}  // namespace perception
}  // namespace avos
