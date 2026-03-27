#include "iou.h"

// 第一步找到最左边的点
static int find_leftmost_point(std::vector<cv::Point2f>& intersectingRegion)
{
    int index = 0;
    float tmp = intersectingRegion[0].x;
    for(int i=1; i<intersectingRegion.size(); i++)
    {
        if(intersectingRegion[i].x < tmp)
        {
            tmp = intersectingRegion[i].x;
            index = i;
        }
    }
    return index;
}

//第二步对所有点进行排序
static std::vector<cv::Point2f> sort_points(std::vector<cv::Point2f>& intersectingRegion)
{
	std::vector<cv::Point2f> sort_intersectingRegion;
    int leftmost_index = find_leftmost_point(intersectingRegion);

    std::vector<float> arctan;
    for(int i=0; i<intersectingRegion.size(); i++)
    {
        arctan.push_back(atan2(intersectingRegion[i].x - intersectingRegion[leftmost_index].x, intersectingRegion[i].y - intersectingRegion[leftmost_index].y));
    }

    std::vector<int> index;
    for(int i=0; i<arctan.size(); i++)
    {
        index.push_back(i);
    }

    std::sort(index.begin(), index.end(), [&](const int& a, const int& b) {return (arctan[a] < arctan[b]);});

    for(int i=0; i<index.size(); i++)
    {
        sort_intersectingRegion.push_back(intersectingRegion[index[i]]);
    }
    return sort_intersectingRegion;
}


void DrawBox(cv::RotatedRect box,cv::Mat img){

    // CvPoint2D32f point[4];
    // int i;
    // for (i=0; i<4; i++){
    //     point[i].x = 0;
    //     point[i].y = 0;
    // }
    // cvBoxPoints(box, point); //计算二维盒子顶点
    // cv::Point pt[4];

    // for (i=0; i<4; i++){
    //     pt[i].x = (int)point[i].x;
    //     pt[i].y = (int)point[i].y;
    // }
    cv::Mat point;
    int i;
    /*for (i=0; i<4; i++){
        point[i].x = 0;
        point[i].y = 0;
    }*/
    cv::boxPoints(box, point); //计算二维盒子顶点
    cv::Point pt[4];

    for (i=0; i<4; i++){
        pt[i].x = (int)point.at<float>(i,0);
        pt[i].y = (int)point.at<float>(i,1);
    }

    cv::line(img, pt[0], pt[1],CV_RGB(255,255,255), 1, 8, 0 );
    cv::line(img, pt[1], pt[2],CV_RGB(255,255,255), 1, 8, 0 );
    cv::line(img, pt[2], pt[3],CV_RGB(255,255,255), 1, 8, 0 );
    cv::line(img, pt[3], pt[0],CV_RGB(255,255,255), 1, 8, 0 );
}

double RotateIou(cv::RotatedRect& r1, cv::RotatedRect& r2){
	std::vector<cv::Point2f> intersectingRegion;
	double intersectionType = cv::rotatedRectangleIntersection(r1, r2, intersectingRegion);
	double inter_area = 0;
	// cout<<"intersectingRegion "<<intersectingRegion.size()<<endl;

	if (intersectingRegion.empty()){
		inter_area = 0;
	}
	else{
		std::vector<cv::Point2f> sort_intersectingRegion = sort_points(intersectingRegion);
		inter_area = cv::contourArea(sort_intersectingRegion);
	}

	cv::Size2f r1size = r1.size;
	cv::Size2f r2size = r2.size;
	double area1 = r1size.area();
	double area2 = r2size.area();
	// double iou = inter_area/(area1 + area2 - inter_area+ 0.00000001);
	double iou = inter_area/(std::min(area1, area2) + 1e-6);
	return iou;
};


double Iou(Box2D& box1, Box2D& box2){


    // std::cout<<"box2 "<<box2.lower_rightx<<" "<<box2.lower_righty<<" "<<box2.upper_leftx<<" "<<box2.upper_lefty<<std::endl;
    // std::cout<<"box2d "<<box1.lower_rightx<<" "<<box1.lower_righty<<" "<<box1.upper_leftx<<" "<<box1.upper_lefty<<std::endl;

    float area1 = (box1.lower_rightx - box1.upper_leftx) * (box1.lower_righty - box1.upper_lefty);
    float area2 = (box2.lower_rightx - box2.upper_leftx) * (box2.lower_righty - box2.upper_lefty);

    float xmin = std::max(box1.upper_leftx, box2.upper_leftx);
    float xmax = std::min(box1.lower_rightx, box2.lower_rightx);
    float ymin = std::max(box1.upper_lefty, box2.upper_lefty);
    float ymax = std::min(box1.lower_righty, box2.lower_righty);
    // std::cout<<"xmin "<<xmin<<" xmax "<<xmax<<" ymin "<<ymin<<" ymax "<<ymax<<std::endl;

    if((xmax - xmin) <= 0 || (ymax-  ymin)<=0)
        return 0;
    
    float inter_area = (xmax - xmin) * (ymax-  ymin);

    // std::cout<<"area1 "<<area1<<" area2 "<<area2<<" "<<inter_area<<std::endl;
    return inter_area / (area1 + area2 - inter_area +1e-6);
}

