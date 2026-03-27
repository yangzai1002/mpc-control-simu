#pragma once

#include <vector>
#include <deque>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <opencv2/core.hpp>

namespace avos {
namespace perception {
namespace algorithm {

double GetRange(const double& x,
                const double& y);

double GetDistance(const double &x1,
                            const double &y1,
                            const double &x2,
                            const double &y2);
double GetDistance(const double &x1,
                            const double &y1,
                            const double &z1,
                            const double &x2,
                            const double &y2,
                            const double &z2);
double GetEuclideanDistance(const double &x1,
                            const double &y1,
                            const double &x2,
                            const double &y2);

double GetCosineDistance(const double &x1,
                         const double &y1,
                         const double &x2,
                         const double &y2);

float GetSpeed(const float &vx,
               const float &vy);

void GetLatLonVelocity(const float &vx,
                       const float &vy,
                       const float &heading_deg,
                       float &v_lat,
                       float &v_lon);

float NormalizeHeading(const float &in_deg);

float GetMeanHeading(const std::deque<float> &buffer);

float GetMean(const std::deque<float> &buffer);

float GetRMS(const std::deque<float> &buffer);

void GetMeanAndRMS(const std::deque<float> &buffer,
                   float &mean,
                   float &rms);
// heading融合工具，为避免在跨越±180度处出问题，先把角度转换成向量，对向量融合完再转换回角度
cv::Vec2f angle2vec(float angle);

cv::Vec2f vecplus(cv::Vec2f vec1, cv::Vec2f vec2);

float vec2angle(cv::Vec2f vec);

float vecdelta(cv::Vec2f vec1, cv::Vec2f vec2);
bool judgeinrect(std::vector<cv::Point2f> pts, cv::Point2f point);
bool projectPointOnLineSegment(cv::Point2f point,cv::Point2f projection_point,double &dist, cv::Point2f line_start, cv::Point2f line_end);
int distanceToRotatedRectEdges(cv::Point2f point, cv::RotatedRect rotated_rect);
bool fitLine(std::vector<cv::Point2f>& points, float& slope,float& intercept);
bool ransacPlaneFitting(const std::vector<cv::Point3f>& points, cv::Vec4f& plane_coeffs,  std::vector<int>& inliers, int iterations = 100,  float threshold = 0.1);

// 数组里应该按照[x_min, y_min, x_max, y_max]的形式排列 
std::vector<double> getBoundingBox(std::vector<double> boxA, std::vector<double> boxB) ;

double area(std::vector<double> box) ;

std::vector<double> intersection(std::vector<double> boxA, std::vector<double> boxB) ;

bool GIOU(cv::Rect2f rect1,cv::Rect2f rect2,double &giou) ;

}  // namespace algorithm
}  // namespace perception
}  // namespace avos

