#ifndef IOU_H
#define IOU_H

#include <iostream>
#include <string>
#include <vector>
#include<algorithm>

// #include <opencv2/opencv.hpp>
//#include <opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h> 

struct Box2D{
    int upper_leftx;
    int upper_lefty;
    int lower_rightx;
    int lower_righty;
    int width;
    int height;
    int centerx;
    int centery;
};

struct point{
    double x;
    double y;
};

struct Box3D{
    float width;
    float length;
    float height;
    float centerx;
    float centery;
    float centerz;
    float meanx;
    float meany;
    float yaw;
    double globalx;
    double globaly;
    point corner_points[4];
};

// 第一步找到最左边的点
static int find_leftmost_point(std::vector<cv::Point2f>& intersectingRegion);
//第二步对所有点进行排序
static std::vector<cv::Point2f> sort_points(std::vector<cv::Point2f>& intersectingRegion);
void DrawBox(cv::RotatedRect box,cv::Mat img);
double RotateIou(cv::RotatedRect& r1, cv::RotatedRect& r2);
double Iou(Box2D& box1, Box2D& box2);

#endif