#pragma once

#include <opencv2/core.hpp>

namespace avos {
namespace perception {
namespace algorithm {


template <typename T>
inline bool isInside(T p, cv::Mat& img, int boarder = 1) {
    return p.x >= boarder && p.x < img.cols-boarder && p.y >= boarder && p.y < img.rows-boarder;
}

template <typename T>
inline bool isInside(T p, cv::Size size, int boarder = 1) {
    return p.x >= boarder && p.x < size.width-boarder && p.y >= boarder && p.y < size.height-boarder;
}


inline bool IsPointInRect(const cv::Rect2f & rect, const cv::Point2f & pt) {
  if (pt.x >= rect.tl().x && pt.x < rect.br().x
      && pt.y >= rect.tl().y && pt.y < rect.br().y) {
    return true;
  }
  return false;
}
inline bool IsPointOutRect(const cv::Rect2f & rect, const cv::Point & pt) {
  if (pt.x < rect.tl().x || pt.x > rect.br().x
      || pt.y < rect.tl().y || pt.y > rect.br().y) {
    return true;
  }
  return false;
}
inline bool IsPointInRectint(const cv::Rect2f & rect, const cv::Point & pt) {
  if (pt.x >= (int)rect.tl().x && pt.x < (int)rect.br().x
      && pt.y >= (int)rect.tl().y && pt.y < (int)rect.br().y) {
    return true;
  }
  return false;
}
inline cv::Rect2f rect2Crop(cv::Rect2f rect,float sx,float sy,float tx,float ty)
{
  if(sx==0||sy==0) return rect;
  int u = (rect.x - tx) / sx;
  int v = (rect.y - ty) / sy;
  int w = rect.width / sx;
  int h = rect.height / sy;
  return cv::Rect2f(u, v, w, h);
}
//type == 0 resize width type ==1 resize height;
//type == -1 ,both width and height;
inline cv::Rect2f resizeRect(const cv::Rect2f &rect, float scale,int type = -1)
{
  cv::Rect2f _resizeRect = rect;
  if (scale == 1)
    return _resizeRect;
  if (type == -1)
  {
    _resizeRect.x = rect.x - rect.width * ((scale - 1) / 2);
    _resizeRect.y = rect.y - rect.height * ((scale - 1) / 2);
    _resizeRect.width = rect.width * (scale);
    _resizeRect.height = rect.height * (scale);
  }
  else if (type == 0) // resize width
  {
    _resizeRect.x = rect.x - rect.width * ((scale - 1) / 2);
    _resizeRect.y = rect.y;
    _resizeRect.width = rect.width * (scale);
    _resizeRect.height = rect.height;
  }
  else if (type == 1) // resize height
  {
    _resizeRect.x = rect.x;
    _resizeRect.y = rect.y - rect.height * ((scale - 1) / 2);
    _resizeRect.width = rect.width;
    _resizeRect.height = rect.height * (scale);
  }

  return _resizeRect;
}



}  // namespace algorithm
}  // namespace perception
}  // namespace avos


