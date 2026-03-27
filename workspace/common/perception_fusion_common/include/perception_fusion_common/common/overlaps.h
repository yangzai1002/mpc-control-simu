#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>   // cv::line
#include <opencv2/highgui.hpp>   // cv::imshow, cv::waitKey
#include <iostream>
#include <thread>
#include <iomanip>  

namespace avos {
namespace perception {

float calcIOU(const cv::RotatedRect &rect1, const cv::RotatedRect &rect2, int type);
float calcInterArea(const cv::RotatedRect &rect1, const cv::RotatedRect &rect2);


class Overlaps {

 public:
  // std::mutex mutex_;
  // type=1：标准iou   type=2：交集/较小矩形面积  type=3: 交集面积
  // boxes[5]: x_center, y_center, w, h(along heading), heading(deg)
  float Compute(const float* boxes, const float* query_boxes, int type = 1) {
    // std::unique_lock<std::mutex> lock(mutex_);
    // // 注：inter函数理论上应该返回最多8个交点，但用下面这组输入，求得的交点数量会大于8个，之前int_pts[16]长度为16会溢出，导致崩溃，改为32解决。
    // // 另外，交点数量大于8个时，算得iou可能会大于1，所以增加1的上限。
    // float b1[] = {-57.882793,16.438337,1.800697,4.496599,168.381577};
    // float b2[] = {-57.882793,16.438337,1.800695,4.496621,-11.618427};
    // float rr = rotate_iou(b1, b2);
    if(type == 1) return rotate_iou(boxes, query_boxes);
    else if(type == 2) return rotate_iomin(boxes, query_boxes);
    else if(type == 3) return rotate_inter_area(boxes, query_boxes);
    else return -1;
  }

 private:
  float rotate_iou(float const * const region1, float const * const region2) {
    // if((fabs(region1[0] - region2[0]) < 1e-5)
    // && (fabs(region1[1] - region2[1]) < 1e-5)
    // && (fabs(region1[2] - region2[2]) < 1e-5)
    // && (fabs(region1[3] - region2[3]) < 1e-5)
    // && (fabs(region1[4] - region2[4]) < 1e-5)) {
    //     return 1.0;
    // }

    float pts1[8] = {0.0};
    float pts2[8] = {0.0};
    float int_pts[32] = {0.0};
    int num_of_inter = 0;
    float area_inter = inter(region1, region2, pts1, pts2, int_pts, num_of_inter);

    float area1 = region1[2] * region1[3];
    float area2 = region2[2] * region2[3];
    if((area1 + area2 - area_inter) < 1e-5) return 0;
    float result = area_inter / (area1 + area2 - area_inter);

    if (result < 0) {
      result = 0.0;
    }
    else if(result > 1) {
      result = 1;
    }

    // print_details(region1, region2, pts1, pts2, int_pts, num_of_inter);
    // show_box_img(pts1, pts2);

    return result;
  }

  float rotate_iomin(float const * const region1, float const * const region2) {
    float pts1[8] = {0.0};
    float pts2[8] = {0.0};
    float int_pts[32] = {0.0};
    int num_of_inter = 0;
    float area_inter = inter(region1, region2, pts1, pts2, int_pts, num_of_inter);

    float area1 = region1[2] * region1[3];
    float area2 = region2[2] * region2[3];
    if(fmin(area1, area2) < 1e-5) return 0;
    float result = area_inter / fmin(area1, area2);

    if (result < 0) {
      result = 0.0;
    }
    else if(result > 1) {
      result = 1;
    }

    return result;
  }

  float rotate_inter_area(float const * const region1, float const * const region2) {
    float pts1[8] = {0.0};
    float pts2[8] = {0.0};
    float int_pts[32] = {0.0};
    int num_of_inter = 0;
    float area_inter = inter(region1, region2, pts1, pts2, int_pts, num_of_inter);

    if (area_inter < 0) {
      area_inter = 0.0;
    }
    return area_inter;
  }

  float inter(float const * const region1, float const * const region2,
              float * pts1, float * pts2, float * int_pts, int &num_of_inter) {

    convert_region(pts1, region1);
    convert_region(pts2, region2);

    num_of_inter = inter_pts(pts1, pts2, int_pts);

    reorder_pts(int_pts, num_of_inter);

    return area(int_pts, num_of_inter);
  }

  void convert_region(float * pts , float const * const region) {
    auto normalize = [](const float & in_deg) {
      auto out_deg = fmod(in_deg, 360.0);
      if (out_deg < -180.0) { out_deg += 360.0; }
      if (out_deg > 180.0) { out_deg -= 360.0; }
      return out_deg;
    };
    float angle_rad = normalize(region[4]) / 180.0 * M_PI;
    float a_cos = cos(angle_rad);
    float a_sin = sin(angle_rad); // anti clock-wise

    float ctr_x = region[0];
    float ctr_y = region[1];
    float h = region[2];
    float w = region[3];

    float pts_x[4];
    float pts_y[4];

    pts_x[0] = - w / 2;
    pts_x[1] = - w / 2;
    pts_x[2] = w / 2;
    pts_x[3] = w / 2;

    pts_y[0] = - h / 2;
    pts_y[1] = h / 2;
    pts_y[2] = h / 2;
    pts_y[3] = - h / 2;

    for (int i = 0; i < 4; i++) {
      pts[2 * i] = a_cos * pts_x[i] - a_sin * pts_y[i] + ctr_x;
      pts[2 * i + 1] = a_sin * pts_x[i] + a_cos * pts_y[i] + ctr_y;
    }

  }

  int inter_pts(float * pts1, float * pts2, float * int_pts) {

    int num_of_inter = 0;

    for (int i = 0; i < 4; i++) {
      if (inrect(pts1[2 * i], pts1[2 * i + 1], pts2)) {
        int_pts[num_of_inter * 2] = pts1[2 * i];
        int_pts[num_of_inter * 2 + 1] = pts1[2 * i + 1];
        num_of_inter++;
      }
      if (inrect(pts2[2 * i], pts2[2 * i + 1], pts1)) {
        int_pts[num_of_inter * 2] = pts2[2 * i];
        int_pts[num_of_inter * 2 + 1] = pts2[2 * i + 1];
        num_of_inter++;
      }
    }

    float temp_pts[2];

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        bool has_pts = inter2line(pts1, pts2, i, j, temp_pts);
        if (has_pts) {
          int_pts[num_of_inter * 2] = temp_pts[0];
          int_pts[num_of_inter * 2 + 1] = temp_pts[1];
          num_of_inter++;
        }
      }
    }

    return num_of_inter;
  }

  bool inrect(float pt_x, float pt_y, float * pts) {

    double ab[2];
    double ad[2];
    double ap[2];

    double abab;
    double abap;
    double adad;
    double adap;

    ab[0] = pts[2] - pts[0];
    ab[1] = pts[3] - pts[1];

    ad[0] = pts[6] - pts[0];
    ad[1] = pts[7] - pts[1];

    ap[0] = pt_x - pts[0];
    ap[1] = pt_y - pts[1];

    abab = ab[0] * ab[0] + ab[1] * ab[1];
    abap = ab[0] * ap[0] + ab[1] * ap[1];
    adad = ad[0] * ad[0] + ad[1] * ad[1];
    adap = ad[0] * ap[0] + ad[1] * ap[1];
    // bool result = (abab - abap >=  -1) and (abap >= -1) and (adad - adap >= -1) and (adap >= -1);
    // return result;
    double eps = 1e-4;
    return (abab >= abap - eps) and (abap >= 0 - eps) and (adad >= adap - eps) and (adap >= 0 - eps);
  }

  void reorder_pts(float * int_pts, int num_of_inter) {

    if (num_of_inter > 0) {

      float center[2];

      center[0] = 0.0;
      center[1] = 0.0;

      for (int i = 0; i < num_of_inter; i++) {
        center[0] += int_pts[2 * i];
        center[1] += int_pts[2 * i + 1];
      }
      center[0] /= num_of_inter;
      center[1] /= num_of_inter;

      float vs[16];
      float v[2];
      float d;
      for (int i = 0; i < num_of_inter; i++) {
        v[0] = int_pts[2 * i] - center[0];
        v[1] = int_pts[2 * i + 1] - center[1];
        d = sqrt(v[0] * v[0] + v[1] * v[1]);
        v[0] = v[0] / d;
        v[1] = v[1] / d;
        if (v[1] < 0) {
          v[0] = - 2 - v[0];
        }
        vs[i] = v[0];
      }

      float temp, tx, ty;
      int j;
      for (int i = 1; i < num_of_inter; ++i) {
        if (vs[i - 1] > vs[i]) {
          temp = vs[i];
          tx = int_pts[2 * i];
          ty = int_pts[2 * i + 1];
          j = i;
          while (j > 0 && vs[j - 1] > temp) {
            vs[j] = vs[j - 1];
            int_pts[j * 2] = int_pts[j * 2 - 2];
            int_pts[j * 2 + 1] = int_pts[j * 2 - 1];
            j--;
          }
          vs[j] = temp;
          int_pts[j * 2] = tx;
          int_pts[j * 2 + 1] = ty;
        }
      }
    }

  }

  bool inter2line(float * pts1, float *pts2, int i, int j, float * temp_pts) {

    float a[2];
    float b[2];
    float c[2];
    float d[2];

    float area_abc, area_abd, area_cda, area_cdb;

    a[0] = pts1[2 * i];
    a[1] = pts1[2 * i + 1];

    b[0] = pts1[2 * ((i + 1) % 4)];
    b[1] = pts1[2 * ((i + 1) % 4) + 1];

    c[0] = pts2[2 * j];
    c[1] = pts2[2 * j + 1];

    d[0] = pts2[2 * ((j + 1) % 4)];
    d[1] = pts2[2 * ((j + 1) % 4) + 1];

    area_abc = trangle_area(a, b, c);
    area_abd = trangle_area(a, b, d);

    if (area_abc * area_abd >= -1e-5) {
      return false;
    }

    area_cda = trangle_area(c, d, a);
    area_cdb = area_cda + area_abc - area_abd;

    if (area_cda * area_cdb >= -1e-5) {
      return false;
    }
    float t = area_cda / (area_abd - area_abc);

    float dx = t * (b[0] - a[0]);
    float dy = t * (b[1] - a[1]);
    temp_pts[0] = a[0] + dx;
    temp_pts[1] = a[1] + dy;

    return true;
  }

  float area(float * int_pts, int num_of_inter) {

    float area = 0.0;
    for (int i = 0; i < num_of_inter - 2; i++) {
      area += fabs(trangle_area(int_pts, int_pts + 2 * i + 2, int_pts + 2 * i + 4));
    }
    return area;
  }

  float trangle_area(float * a, float * b, float * c) {
    return ((a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0])) / 2.0;
  }

  void print_details(float const * const region1, float const * const region2,
                     float const * const pts1, float const * const pts2,
                     float const * const int_pts, const int num_of_inter) {
    std::cout << "+++ box1: " << std::endl;
    std::cout << region1[0] << ", " << region1[1] << ", width:" << region1[2]
              << ", length:" << region1[3] << ", heading:" << region1[4] << std::endl;

    std::cout << "+++ box2: " << std::endl;
    std::cout << region2[0] << ", " << region2[1] << ", width:" << region2[2]
              << ", length:" << region2[3] << ", heading:" << region2[4] << std::endl;

    std::cout << "+++ pts1: " << std::endl;
    for (int i = 0; i < 4; ++i) {
      std::cout << "(" << pts1[i * 2] << "," << pts1[i * 2 + 1] << "), ";
    }
    std::cout << std::endl;
    std::cout << "+++ pts2: " << std::endl;
    for (int i = 0; i < 4; ++i) {
      std::cout << "(" << pts2[i * 2] << "," << pts2[i * 2 + 1] << "), ";
    }
    std::cout << std::endl;

    std::cout << "+++ int_pts: " << num_of_inter << std::endl;
    for (int i = 0; i < num_of_inter; ++i) {
      std::cout << "(" << int_pts[i * 2] << "," << int_pts[i * 2 + 1] << "), ";
    }
    std::cout << std::endl;
  }

  void show_box_img(float const * const pts1, float const * const pts2) {
    /// Create an image
    float scale = 20;

    int max_x = int(std::max({pts1[0], pts1[2], pts1[4], pts1[6]}));
    int max_y = int(std::max({pts1[1], pts1[3], pts1[5], pts1[7]}));

    int r = scale * ( std::max(max_x, max_y) + 5);
    cv::Mat src = cv::Mat::zeros( cv::Size( r, r ), CV_8UC3 );

    /// Create a sequence of points to make a contour:
    std::vector<cv::Point2f> vert(4);

    vert[0] = cv::Point( pts1[0] * scale, pts1[1] * scale);
    vert[1] = cv::Point( pts1[2] * scale, pts1[3] * scale);
    vert[2] = cv::Point( pts1[4] * scale, pts1[5] * scale);
    vert[3] = cv::Point( pts1[6] * scale, pts1[7] * scale);
    /// Draw it in src
    for ( int j = 0; j < 4; j++ )
    { cv::line( src, vert[j],  vert[(j + 1) % 4], cv::Scalar( 255, 0, 0 ), 1, 8 ); }

    vert[0] = cv::Point( pts2[0] * scale, pts2[1] * scale);
    vert[1] = cv::Point( pts2[2] * scale, pts2[3] * scale);
    vert[2] = cv::Point( pts2[4] * scale, pts2[5] * scale);
    vert[3] = cv::Point( pts2[6] * scale, pts2[7] * scale);
    /// Draw it in src
    for ( int j = 0; j < 4; j++ )
    { cv::line( src, vert[j],  vert[(j + 1) % 4], cv::Scalar( 0, 255, 0 ), 1, 8 ); }

    cv::imshow("src", src );
    cv::waitKey(0);
  }

};
}
}
