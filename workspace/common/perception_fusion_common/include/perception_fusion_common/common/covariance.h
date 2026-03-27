#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>   // cv::line
#include <opencv2/highgui.hpp>   // cv::imshow, cv::waitKey

namespace avos {
namespace perception {


inline cv::Matx22f getCovMat1(float l, float s, float angle) {
  cv::Matx22f D(l*l,0,0,s*s);
  float theta = angle*CV_PI/180;
  cv::Matx22f V(cos(theta), -sin(theta), sin(theta), cos(theta)); // 每列是特征向量
  cv::Matx22f C = V*D*V.t();
  return C;
}

inline cv::RotatedRect getRRectFromCovMat1(cv::Matx22f C, cv::Point2f center, float scale) {
  cv::Vec2f eigenvalues;
  cv::Matx22f eigenvectors;
  eigen(C, eigenvalues, eigenvectors); // 每行是一个特征向量
  float angle = atan2(eigenvectors(0,1), eigenvectors(0,0));
  cv::RotatedRect rrect = cv::RotatedRect(center, cv::Size2f(sqrt(fabs(eigenvalues[0]))*scale *2, sqrt(fabs(eigenvalues[1]))*scale *2), angle*57.3);
  return rrect;
}

// 点到椭圆的归一化距离
inline float ellipseDist(cv::RotatedRect rrect, cv::Point2f p) {
  cv::Point2f pp = p - rrect.center;
  float a = rrect.angle*CV_PI/180;
  float w = rrect.size.width/2;
  float h = rrect.size.height/2;
  cv::Matx22f rot(cos(a), sin(a), -sin(a), cos(a));
  cv::Point2f pr = rot*pp;
  float ret = pr.x*pr.x/w/w + pr.y*pr.y/h/h;
  return ret;
}

inline bool isInsideEllipse(cv::RotatedRect rrect, cv::Point2f p) {
  float ret = ellipseDist(rrect,p);
  return ret <= 1;
}

}
}
