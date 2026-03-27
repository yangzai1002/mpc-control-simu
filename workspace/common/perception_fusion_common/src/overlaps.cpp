#include "perception_fusion_common/common/overlaps.h"
#include <string>

namespace avos {
namespace perception {

using namespace cv;
using namespace std;






// type=1：标准iou   type=2：交集/较小矩形面积
float calcIOU(const cv::RotatedRect &rect1, const cv::RotatedRect &rect2, int type) {
  Overlaps m;
  // x_center, y_center, w, h(along heading), heading(deg)
  float org_boxes[5] = {
    rect1.center.x,
    rect1.center.y,
    rect1.size.height,
    rect1.size.width,
    rect1.angle,
  };
  float query_boxes[5] = {
    rect2.center.x,
    rect2.center.y,
    rect2.size.height,
    rect2.size.width,
    rect2.angle,
  };

  return m.Compute(org_boxes, query_boxes, type);
}


float calcInterArea(const cv::RotatedRect &rect1, const cv::RotatedRect &rect2) {
  Overlaps m;
  // x_center, y_center, w, h(along heading), heading(deg)
  float org_boxes[5] = {
    rect1.center.x,
    rect1.center.y,
    rect1.size.height,
    rect1.size.width,
    rect1.angle,
  };
  float query_boxes[5] = {
    rect2.center.x,
    rect2.center.y,
    rect2.size.height,
    rect2.size.width,
    rect2.angle,
  };

  return m.Compute(org_boxes, query_boxes, 3);
}


// float calcIOU(cv::RotatedRect rect1, cv::RotatedRect rect2) {
//   float areaRect1 = rect1.size.width * rect1.size.height;
//   float areaRect2 = rect2.size.width * rect2.size.height;
//   vector<cv::Point2f> vertices;

//   int intersectionType = cv::rotatedRectangleIntersection(rect1, rect2, vertices);
//   if (vertices.size()==0)
//     return 0.0;
//   else{
//     vector<cv::Point2f> order_pts;
//     // 找到交集（交集的区域），对轮廓的各个点进行排序
//     cv::convexHull(cv::Mat(vertices), order_pts, true);
//     double area = cv::contourArea(order_pts);
//     float inner = (float) (area / (areaRect1 + areaRect2 - area + 0.0001));
//     return inner;
//   }
// }


// float calcIOUSmall(cv::RotatedRect rect1, cv::RotatedRect rect2) {
//   float areaRect1 = rect1.size.width * rect1.size.height;
//   float areaRect2 = rect2.size.width * rect2.size.height;
//   vector<cv::Point2f> vertices;
//   std::cout << "dddddd: " << rect1.size.width << " " << rect1.size.height << " " << rect2.size.width << " " << rect2.size.height << std::endl;
//   //  what():  /build/opencv-L2vuMj/opencv-3.2.0+dfsg/modules/imgproc/src/intersection.cpp:247: error: (-215) intersection.size() <= 8 in function rotatedRectangleIntersection
//   int intersectionType = cv::rotatedRectangleIntersection(rect1, rect2, vertices);
//   if (vertices.size()==0)
//     return 0.0;
//   else{
//     vector<cv::Point2f> order_pts;
//     // 找到交集（交集的区域），对轮廓的各个点进行排序
//     cv::convexHull(cv::Mat(vertices), order_pts, true);
//     double area = cv::contourArea(order_pts);
//     float inner = (float) (area / (fmin(areaRect1, areaRect2) + 0.0001));
//     return inner;
//   }
// }



}
}
