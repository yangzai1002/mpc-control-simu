#include "map_fusion.h"
using namespace std;
MapFusion::MapFusion(int ogm_size, int img_width, int img_height, 
                     int vehicle_width, int vehicle_length,
                     int vehicle_posx, int vehicle_posy){
    grid_size_ = ogm_size;
    frame_ = 0;
    pre_ogm_.resize(grid_size_);


    imgwidth = img_width;
    imgheight = img_height;

    char *buffer;
    std::string buffers;
    buffer = getcwd(NULL, 0);
    if(buffer == NULL){
        perror("getcwd error");
    }
    else{
        buffers = buffer;
        // printf("[INFO] enhance path: \n %s\n", buffer);
        free(buffer);
    }

    std::string config_path;
    // boost::char_separator<char> sep { "/" };
    // tokenizer tokn(buffers, sep);
    // std::vector<std::string> path(tokn.begin(), tokn.end());

    // for(int i=0; i<path.size()-2; ++i){
	//   config_path += "/" + path[i];
    // } 
    
    const char *CONFIG_FILE_PATH = std::getenv("MAP_FUSION_FILE_PATH");
    printf("CONFIG_FILE_PATH = %s\n", CONFIG_FILE_PATH);

    config_path = CONFIG_FILE_PATH;
    std::cout<<"[CUSTOM INFO] config path "<<config_path<<"\n";

  	// config_path = "/home/wx/Documents/mywork/lidar_ground/terrain_seg/config/mapfusion_param.xml";
    boost::property_tree::ptree pt;
    boost::property_tree::read_xml(config_path, pt);
	HitgirdP  = pt.get<double>("ocu_prob");
	NullgridP = pt.get<double>("null_prob");
	BlindNullGridP  = pt.get<double>("blind_prob");

    R_Ins_ = Eigen::MatrixXd(2, 2);
    H_Ins_ = Eigen::MatrixXd(2, 4);
    //measurement covariance matrix - laser
    R_Ins_ << 0.0225, 0,
                0, 0.0225;

    H_Ins_ << 1, 0, 0, 0,
                0, 1, 0, 0;

    kf_.P_ = Eigen::MatrixXd(4, 4);
    kf_.P_ <<   1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 100, 0,
                0, 0, 0, 100;

    kf_.F_ = Eigen::MatrixXd(4, 4);
    kf_.F_ <<   1, 0, 1, 0,
                0, 1, 0, 1,
                0, 0, 1, 0,
                0, 0, 0, 1;

    noise_ax = 0.05;
    noise_ay = 0.05;

    wrd2img = Eigen::Vector3d(15, 15, 0);

    mask_img = cv::Mat::zeros(img_height, img_width, CV_8UC1);

    // int v_posy = 75;
    // int v_posx = 75;
    int offsety = img_width * tan(30.5 * M_PI/180) /2 + vehicle_posy;

    std::vector<cv::Point> mask_pts = {cv::Point(0, 0), 
                                        cv::Point(0, offsety), 
                                        cv::Point(img_width/2, vehicle_posy), 
                                        cv::Point(img_width, offsety), 
                                        cv::Point(img_width, 0)};

    std::vector<std::vector<cv::Point>> polygons;
    polygons.push_back(mask_pts);
    cv::fillPoly(mask_img, polygons, 255);


    vehicle_width_ = vehicle_width;
    vehicle_length_ = vehicle_length;
    vehicle_posx_ = vehicle_posx; 
    vehicle_posy_ = vehicle_posy;

    cout<<"vehicle_posx "<<vehicle_posx<<" vehicle_width "<<vehicle_width
        <<" vehicle_posy "<<vehicle_posy<<" vehicle_length "<<vehicle_length<<"\n";

	cv::rectangle(mask_img, 
                cv::Point(vehicle_posx - vehicle_width/2,  vehicle_posy + vehicle_length/2),
                cv::Point(vehicle_posx + vehicle_width/2,  vehicle_posy - vehicle_length/2), 
                CV_RGB(0,0,0), -1);
}

void MapFusion::FusionProcess(DMAP& ogm_in, 
                    std::vector<unsigned char>& ogm_out,
                    std::vector<float>& ogm_fusion_minh,
                    std::vector<float>& ogm_fusion_maxh,
                    std::vector<double>& params,
                    long long int time){
    if(frame_ == 0){
        for(int i=0;i <ogm_in.lab.size(); ++i){
            if(static_cast<int>(ogm_in.lab[i]) == NONTRAVESABLE){
                pre_ogm_[i].possibility = HitgirdP;
				pre_ogm_[i].hitcount++;
                pre_ogm_[i].maxh = ogm_in.demhmax[i];
                pre_ogm_[i].minh = ogm_in.demhmin[i];
            }else if(static_cast<int>(ogm_in.lab[i]) == TRAVESABLE){
                if(ogm_in.demhnum[i] != 0){
                    pre_ogm_[i].maxh = ogm_in.demhmax[i];
                    pre_ogm_[i].minh = ogm_in.demhmin[i];
                }
            }
        }

        kf_.x_ = Eigen::VectorXd(4);
        kf_.x_ << params[0], params[1], 0, 0;  //x, y, vx, vy  
        pre_time_ = time;

        Eigen::AngleAxisd rotp(params[2] * M_PI/180, Eigen::Vector3d::UnitZ());
        rotation_pre_ = rotp.toRotationMatrix();
        t_pre_ = Eigen::Vector3d(params[0], params[1], 0);

        frame_++;
        return;
    }

    double dt = (time - pre_time_) / 1000.0;
    pre_time_ = time;

    double dt_2 = dt * dt;
    double dt_3 = dt_2 * dt;
    double dt_4 = dt_3 * dt;

    // Modify the F matrix so that the time is integrated
    kf_.F_(0, 2) = dt;
    kf_.F_(1, 3) = dt;
    //set the process covariance matrix Q
    kf_.Q_ = Eigen::MatrixXd(4, 4);
    kf_.Q_ << dt_4/4*noise_ax,   0,                dt_3/2*noise_ax,  0,
             0,                 dt_4/4*noise_ay,  0,                dt_3/2*noise_ay,
             dt_3/2*noise_ax,   0,                dt_2*noise_ax,    0,
             0,                 dt_3/2*noise_ay,  0,                dt_2*noise_ay;

    kf_.Predict();
    kf_.H_ = H_Ins_;
    kf_.R_ = R_Ins_;
    Eigen::VectorXd meas_utm(2);
    meas_utm << params[0], params[1];
    kf_.Update(meas_utm);

    Eigen::AngleAxisd rotn(params[2] * M_PI/180, Eigen::Vector3d::UnitZ());
    rotation_now_ = rotn.toRotationMatrix();
    t_now_ = Eigen::Vector3d(kf_.x_(0), kf_.x_(1), 0);

	ogm_fusion_.resize(grid_size_);

    for (int k =0; k<ogm_fusion_.size(); ++k){
        ogm_fusion_[k].possibility = 0.5;
    }

    // cv::Mat fusionimage = cv::Mat::zeros(imgheight, imgwidth, CV_8UC3);

	for (int i = 0; i < imgwidth; ++i){
		for (int j = 0; j < imgheight - 50; ++j){ //更新前40m
			int dataindex = j * imgwidth + i;
			Eigen::Vector3d p_pre(i , j , 0);
            Eigen::Vector3d TransformZtoL;
            TransformZtoL = (rotation_now_ * ( rotation_pre_.inverse() * (p_pre * 0.2 - wrd2img) + t_pre_  - t_now_) + wrd2img)/0.2;

			int hx = round(TransformZtoL(0));
			int hy = round(TransformZtoL(1));

			int index = hy * imgwidth + hx;
			if (hx > 0 && hx < imgwidth && hy > 0 && hy < imgheight) {
			 	ogm_fusion_[index] = pre_ogm_[dataindex];
			}
		}
	}

	for (int i = 0; i < imgwidth; ++i){
		for (int j = 0; j < imgheight; ++j){
			int index = j * imgwidth + i;        //1代表占据，由于二值贝叶斯更新的缘故，可行驶区域会一直降低概率，可能导致突然出现的物体无法出现在栅格图中
            if ((static_cast<int>(ogm_in.lab[index]) == NONTRAVESABLE)) {
                ogm_fusion_[index].possibility = BayesUpdator(ogm_fusion_[index].possibility, HitgirdP,1);
                //ogm_fusion_[index].hitcount++;
                ogm_fusion_[index].miscount=0;
                ogm_fusion_[index].maxh = ogm_in.demhmax[index];
                ogm_fusion_[index].minh = ogm_in.demhmin[index];
                
            }else if(static_cast<int>(ogm_in.lab[index]) == TRAVESABLE) {
                ogm_fusion_[index].miscount=0;
                if(ogm_in.demhnum[index] != 0){
                    ogm_fusion_[index].maxh = ogm_in.demhmax[index];
                    ogm_fusion_[index].minh = ogm_in.demhmin[index];
                }

                if(ogm_fusion_[index].possibility <= 0.2){
                    continue;
                }

                if(mask_img.at<uchar>(j,i) != 255){
                    ogm_fusion_[index].possibility = BayesUpdator(ogm_fusion_[index].possibility, NullgridP,1);//ogm_fusion_[index].miscount)
                }
            }else if(ogm_fusion_[index].possibility > 0.5){ 

                ogm_fusion_[index].miscount++;
                if(ogm_fusion_[index].miscount > 5){
                    ogm_fusion_[index].miscount = 5;
                }

                if(mask_img.at<uchar>(j,i) != 255){
                    ogm_fusion_[index].possibility = BayesUpdator(ogm_fusion_[index].possibility, NullgridP,1);//ogm_fusion_[index].miscount)
                }

                if(ogm_fusion_[index].possibility < 0.5 && ogm_in.demhnum[index] == 0){
                    ogm_fusion_[index].maxh = 0;
                    ogm_fusion_[index].minh = 0;
                }
            }else{
                ogm_fusion_[index].miscount++;
                if(ogm_fusion_[index].miscount > 5){
                    ogm_fusion_[index].miscount = 5;
                }

                if(mask_img.at<uchar>(j,i) != 255 
                    && ogm_in.demhnum[index] == 0
                    && ogm_fusion_[index].miscount >= 5){

                    ogm_fusion_[index].maxh = 0;
                    ogm_fusion_[index].minh = 0;
                }
            }
        }
    }


    for(int index =  0; index < ogm_fusion_.size(); ++index){
        if (ogm_fusion_[index].possibility >  0.5) {
            ogm_out[index] = NONTRAVESABLE;
        }else if(ogm_fusion_[index].possibility == 0.5){
            ogm_out[index] = UNKNOWN;
        }else{
            ogm_out[index] = TRAVESABLE;
        }

        ogm_fusion_minh[index] = ogm_fusion_[index].minh;
        ogm_fusion_maxh[index] = ogm_fusion_[index].maxh;
    }
       
    // // 历史帧栅格，红色
	// for (int i = 0; i < imgwidth; ++i) {
	// 	for (int j = 0; j < imgheight; ++j) {
	// 		int index = j * imgwidth + i;
	// 		if (ogm_fusion_[index].possibility >  0.5) {
	// 			fusionimage.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 0,255);
    //         }else if(ogm_fusion_[index].possibility == 0.5){
	// 			fusionimage.at<cv::Vec3b>(j, i) = cv::Vec3b(250, 250,250);
    //         }else{
	// 			fusionimage.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 200,0);
    //         }
    //     }
    // }

    // for (int i = 0; i < imgwidth; ++i) {
	// 	for (int j = 0; j < imgheight; ++j) {
	// 		int index = j * imgwidth + i;
	// 		if ((static_cast<int>(ogm_in.lab[index]) == NONTRAVESABLE)) {
    //             fusionimage.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 255, 255);
    //         }
    //     }
    // }

    for(int i=0; i<ogm_fusion_.size(); ++i){
        pre_ogm_[i] = ogm_fusion_[i];
    }

    rotation_pre_ = rotation_now_;
    t_pre_ = t_now_;

    frame_++;

    // float vehicle_pixel_positionx = 15/0.2;
    // float vehicle_pixel_positiony = imgheight -  15/0.2;
    // float vehicle_length = 2/0.2;
    // float vehicle_width = 1/0.2;

    // cv::flip(fusionimage,fusionimage, 0);
    // cv::rectangle(fusionimage,
    //             cv::Point(vehicle_pixel_positionx - vehicle_width/2,  vehicle_pixel_positiony + vehicle_length/2),
    //             cv::Point(vehicle_pixel_positionx + vehicle_width/2,  vehicle_pixel_positiony - vehicle_length/2),
    //             CV_RGB(150,150,150), 1);    
    // cv::namedWindow("mapfusion",CV_WINDOW_NORMAL);
    // cv::imshow("mapfusion", fusionimage);
    // cv::waitKey(1);
    // float vehicle_pixel_positionx = 15/0.2;
    // float vehicle_pixel_positiony = imgheight -  15/0.2;
    // float vehicle_length = 2/0.2;
    // float vehicle_width = 1/0.2;

    // cv::flip(fusionimage,fusionimage, 0);
    // cv::rectangle(fusionimage,
    //             cv::Point(vehicle_posx_ - vehicle_width_/2,  imgheight - vehicle_posy_ + vehicle_length_/2),
    //             cv::Point(vehicle_posx_ + vehicle_width_/2,  imgheight - vehicle_posy_ - vehicle_length_/2),
    //             CV_RGB(150,150,150), 1);    
    // cv::namedWindow("mapfusion",cv::WINDOW_NORMAL);
    // cv::imshow("mapfusion", fusionimage);
    // cv::waitKey(1);
}
