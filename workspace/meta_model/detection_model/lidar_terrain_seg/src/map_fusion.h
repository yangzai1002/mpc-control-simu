#ifndef MAP_FUSION_H
#define MAP_FUSION_H

#include <vector>
#include <string>
// #include "lidar_detection.h"
#include "../perception_common/kalman/kalman_filter.hh"
#include "bayes_updator.h"
//EIGEN
#include <Eigen/Core>
#include <Eigen/Geometry>

//OPENCV
// #include <opencv2/video/tracking.hpp> 
#include <opencv2/highgui/highgui.hpp>
// #include <opencv2/opencv.hpp>

//BOOST
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/tokenizer.hpp>
#include "common.h"

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

struct Grid{
   double possibility = 0.5;
   double hitcount = 0;
   double miscount = 0;
   float minh = 0;
   float maxh = 0;
};

class MapFusion{
public:
    MapFusion(){};
    ~MapFusion(){};
    MapFusion(int ogm_size, int img_width, int img_height,
              int vehicle_width, int vehicle_length,
              int vehicle_posx, int vehicle_posy);
    void FusionProcess(DMAP& ogm_in, 
                    std::vector<unsigned char>& ogm_out,
                    std::vector<float>& ogm_fusion_minh,
                    std::vector<float>& ogm_fusion_maxh,
                    std::vector<double>& params,long long int time);
private:
    int grid_size_;
    int imgwidth = 150;
    int imgheight = 325;
    double origion_utm_[4];
    double pre_utm[4];
    int frame_;
    long long int pre_time_;
    std::vector<Grid> pre_ogm_; 
    std::vector<Grid> ogm_fusion_;   

    KalmanFilter kf_;
    Eigen::MatrixXd R_Ins_;
    Eigen::MatrixXd H_Ins_;
    double noise_ax;
    double noise_ay;
    double HitgirdP = 0.62; 
    double NullgridP = 0.42;
    double BlindNullGridP = 0.48;

    //position
	Eigen::Matrix3d rotation_origion_;
    Eigen::Vector3d t_origion_;
	Eigen::Matrix3d rotation_pre_;
    Eigen::Vector3d t_pre_;
	Eigen::Matrix3d rotation_now_;
    Eigen::Vector3d t_now_;    
    Eigen::Vector3d wrd2img;

    int vehicle_width_;
    int vehicle_length_;
    int vehicle_posx_; 
    int vehicle_posy_;

    cv::Mat mask_img;
};

#endif
