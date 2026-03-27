#include "terrain_seg.h"

using namespace std;

TerrainSeg::TerrainSeg(){
    Init();
}

void TerrainSeg::Init(){
    pcl::PointCloud<PointXYZIRT>::Ptr temp_cloud( new pcl::PointCloud<PointXYZIRT>);
    cloud_g = temp_cloud;

    pcl::PointCloud<pcl::PointXYZ>::Ptr temp_cloud2( new pcl::PointCloud<pcl::PointXYZ>);
    cloud_ob = temp_cloud2;

    pcl::PointCloud<pcl::PointXYZ>::Ptr temp_cloud3( new pcl::PointCloud<pcl::PointXYZ>);
    cloud_gr = temp_cloud3;

    pcl::PointCloud<PointXYZIRT>::Ptr temp_cloud4( new pcl::PointCloud<PointXYZIRT>);
    cloud_input = temp_cloud4;

    const char *CONFIG_FILE_PATH = std::getenv("MODEL_FILE_PATH");
    printf("CONFIG_FILE_PATH = %s\n", CONFIG_FILE_PATH);

    std::string  config_path = CONFIG_FILE_PATH;
    std::cout<<"[CUSTOM INFO] config path "<<config_path<<"\n";
    LoadConfig(config_path);

    ogm_cell_size_ = param.ogm_width * param.ogm_height;
    mapfusion_ = std::make_unique<MapFusion>(ogm_cell_size_, param.ogm_width , param.ogm_height, 
                                            param.vehicle_width, param.vehicle_length, 
                                            param.offset_x, param.offset_y);

    dilate_element_ = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    mean_kernel = cv::Mat(3,3, cv::DataType<float>::type, cv::Scalar::all(0.125));

    
    for(int i=0; i<param.ogm_height; ++i){
        for(int j=0; j<param.ogm_width; ++j){
            float dist = sqrt((i - param.offset_x) * (i - param.offset_x) * param.ogm_resolution
                         + (j - param.offset_y) * (j - param.offset_x)* param.ogm_resolution);
            if(dist < 1.0){
                vehicle_around.push_back(std::pair<int,int>(i,j));
            }
        }
    }

    //process lidar thread
    process_thread = new std::thread([this](){
        while(!stop_flag.load()){
            Process();
        }
    });
}

TerrainSeg::~TerrainSeg(){
    stop_flag = true;
    if(process_thread->joinable()){
        process_thread->join();
    }
}

void TerrainSeg::LoadConfig(std::string& config_path){
    try {
        // 1. 创建 Boost 属性树对象
        boost::property_tree::ptree pt;
        
        // 2. 读取 XML 文件
        boost::property_tree::read_xml(config_path, pt);

        // ========== 读取激光雷达参数 ==========
        param.laser_num = pt.get<int>("TerrainSegConfig.LidarParams.laser_num");
        param.w_res = pt.get<float>("TerrainSegConfig.LidarParams.w_res");
        param.max_angle = pt.get<float>("TerrainSegConfig.LidarParams.max_angle");
        param.min_angle = pt.get<float>("TerrainSegConfig.LidarParams.min_angle");
        param.far_range = pt.get<float>("TerrainSegConfig.LidarParams.far_range");
        param.mid_range = pt.get<float>("TerrainSegConfig.LidarParams.mid_range");
        param.far_range_neg = pt.get<float>("TerrainSegConfig.LidarParams.far_range_neg");

        // ========== 读取点云分割参数 ==========
        // 先读角度（度），再转弧度
        float pp_vertical_angle_deg = pt.get<float>("TerrainSegConfig.SegmentParams.pp_vertical_angle_thresh");
        float pp_angle_deg = pt.get<float>("TerrainSegConfig.SegmentParams.pp_angle_thresh");
        
        param.pp_vertical_angle_thresh = pp_vertical_angle_deg * M_PI / 180.0f;
        param.pp_height_thresh = pt.get<float>("TerrainSegConfig.SegmentParams.pp_height_thresh");
        param.pp_height_thresh_far = pt.get<float>("TerrainSegConfig.SegmentParams.pp_height_thresh_far");
        param.pp_horizon_dist_thresh = pt.get<float>("TerrainSegConfig.SegmentParams.pp_horizon_dist_thresh");
        param.pp_angle_thresh = pp_angle_deg * M_PI / 180.0f;
        param.road_lowerbound = pt.get<float>("TerrainSegConfig.SegmentParams.road_lowerbound");
        param.road_upperbound = pt.get<float>("TerrainSegConfig.SegmentParams.road_upperbound");
        param.seg_plane_znorm = pt.get<float>("TerrainSegConfig.SegmentParams.seg_plane_znorm");
        param.road_region_min_point = pt.get<int>("TerrainSegConfig.SegmentParams.road_region_min_point");
        param.vertifactor = pt.get<float>("TerrainSegConfig.SegmentParams.vertifactor");
        param.horizfactor = pt.get<float>("TerrainSegConfig.SegmentParams.horizfactor");

        // ========== 读取栅格地图参数 ==========
        param.ogm_width = pt.get<int>("TerrainSegConfig.OgmParams.ogm_width");
        param.ogm_height = pt.get<int>("TerrainSegConfig.OgmParams.ogm_height");
        param.offset_x = pt.get<int>("TerrainSegConfig.OgmParams.offset_x");
        param.offset_y = pt.get<int>("TerrainSegConfig.OgmParams.offset_y");
        param.ogm_resolution = pt.get<float>("TerrainSegConfig.OgmParams.ogm_resolution");
        param.base_smooth_error = pt.get<float>("TerrainSegConfig.OgmParams.base_smooth_error");
        param.max_smooth_error = pt.get<float>("TerrainSegConfig.OgmParams.max_smooth_error");
        param.smooth_hori_factor = pt.get<float>("TerrainSegConfig.OgmParams.smooth_hori_factor");

        // ========== 读取车辆参数 ==========
        param.vehicle_height = pt.get<float>("TerrainSegConfig.VehicleParams.vehicle_height");
        param.vehicle_width = pt.get<float>("TerrainSegConfig.VehicleParams.vehicle_width");
        param.vehicle_length = pt.get<float>("TerrainSegConfig.VehicleParams.vehicle_length");

        // 正负障碍检测参数
        param.pos_minheight = pt.get<float>("TerrainSegConfig.VehicleParams.pos_minheight");
        param.neg_minheight = pt.get<float>("TerrainSegConfig.VehicleParams.neg_minheight");

        // ========== 计算派生参数 ==========
        param.r_width = (param.max_angle - param.min_angle) / param.w_res;
        param.r_height = param.laser_num;


        param.front_middle_lidar.y = pt.get<float>("TerrainSegConfig.front_middle_lidar.y");

        std::cout << "[Success] 加载XML配置文件：" << config_path << std::endl;

    } catch (const boost::property_tree::xml_parser_error& e) {
        // XML 文件读取错误（如文件不存在、格式错误）
        std::cerr << "[Error] XML文件读取失败：" << e.what() << std::endl;
        std::cerr << "[Warning] 启用默认参数配置" << std::endl;
    } catch (const std::exception& e) {
        // 参数读取错误（如节点缺失、类型不匹配）
        std::cerr << "[Error] 参数解析失败：" << e.what() << std::endl;
        std::cerr << "[Warning] 启用默认参数配置" << std::endl;
    }
}


void TerrainSeg::InputPosition(percep_datatype::Position pos){
    pos_global = pos;
}

void TerrainSeg::GetOgmInfo(std::vector<float>& param_out){
    param_out.push_back(param.ogm_height);
    param_out.push_back(param.ogm_width);
    param_out.push_back(param.ogm_resolution);
    param_out.push_back(param.offset_x);
    param_out.push_back(param.offset_y);
}

void TerrainSeg::RangeSegProcess(){
    range_map.pts.clear();
    range_map.idx.clear();
    range_map.range.clear();
    range_map.regionID.clear();
    range_map.segbuf.clear();

    range_map.pts.resize(param.r_width * param.r_height);
    range_map.idx.resize(param.r_width * param.r_height, -1);
    range_map.range.resize(param.r_width * param.r_height, 0);
    range_map.regionID.resize(param.r_width * param.r_height,0);

    range_map.regnum = 0;

    GenerateRangeMap();
    SmoothMap();
    bool result = ContourSegger();

    if (range_map.regnum) {
        range_map.segbuf = std::vector<SEGBUF>(range_map.regnum, SEGBUF{});
        Region2Seg ();
    }
}

void TerrainSeg::OgmProcess(){
    GenerateLocOgm();
}

void TerrainSeg::Process(){

    std::unique_lock<std::mutex> lock(lidar_mtx);
    ldt.wait(lock, [this](){return data_receive.load();});
    data_receive = false;
    lock.unlock();

    cloud_g->points.clear();
	for (int i = 0;i < cloud_input->points.size();i++){
        if(cloud_input->points[i].x == 0 && cloud_input->points[i].y == param.front_middle_lidar.y){
            continue;
        }else{
            cloud_g->points.push_back(cloud_input->points[i]);
        }
    }

    timeval tm;
    gettimeofday(&tm,NULL);
    int64_t t0 = tm.tv_sec * 1e3 + tm.tv_usec *1e-3;
    RangeSegProcess();

    gettimeofday(&tm,NULL);
    int64_t t1 = tm.tv_sec * 1e3 + tm.tv_usec *1e-3;
    cout<<"cost "<<t1- t0<<"\n";
    OgmProcess();

    gettimeofday(&tm,NULL);
    int64_t t2 = tm.tv_sec * 1e3 + tm.tv_usec *1e-3;
    cout<<"OgmProcess "<<t2- t1<<"\n";

    ogm_fusion.clear();
    ogm_fusion.resize(ogm_cell_size_, UNKNOWN);
    std::vector<float> ogm_fusion_minh(ogm_cell_size_, 0);
    std::vector<float> ogm_fusion_maxh(ogm_cell_size_, 0);

    std::vector<double> fusion_param(3,0);
    UTMResult utm = from_latlon(pos_global.latitude, pos_global.longitude);

    fusion_param[0] = utm.easting;
    fusion_param[1] = utm.northing;
    fusion_param[2] = pos_global.heading;

    int64_t t3 = tm.tv_sec * 1e3 + tm.tv_usec *1e-3;
    cout<<"FusionProcess "<<t3- t2<<"\n";
    cout<<"ogm_map.lab "<<ogm_map.lab.size()<<"\n";
    gettimeofday(&tm,NULL);
    int64_t time = tm.tv_sec *1e3 + tm.tv_usec * 1e-3;
    mapfusion_->FusionProcess(ogm_map, ogm_fusion, ogm_fusion_minh, ogm_fusion_maxh, fusion_param, time);

    cloud_gr->points.clear();
    cloud_ob->points.clear();

    for(int i = 0; i<param.ogm_height; ++i){
        for(int j=0; j<param.ogm_width; ++j){
            int index = i * param.ogm_width + j;
            if(ogm_fusion[index] == TRAVESABLE){
                float subz = (ogm_fusion_maxh[index] - ogm_fusion_minh[index]);
                if(subz == 0){
                    pcl::PointXYZ p;
                    p.x = (j + 0.5) * param.ogm_resolution- param.offset_x * param.ogm_resolution;
                    p.y = (i + 0.5) * param.ogm_resolution- param.offset_y * param.ogm_resolution;
                    p.z = (ogm_fusion_minh[index]);
                    cloud_gr->points.push_back(p);
                }else{
                    for(int k =0; k<subz/param.ogm_resolution; ++k){
                        pcl::PointXYZ p;
                        p.x = (j + 0.5) * param.ogm_resolution- param.offset_x * param.ogm_resolution;
                        p.y = (i + 0.5) * param.ogm_resolution- param.offset_y * param.ogm_resolution;
                        p.z = (ogm_fusion_minh[index]) + k * param.ogm_resolution;
                        cloud_gr->points.push_back(p);
                    }
                }
            }else if(ogm_fusion[index] == NONTRAVESABLE){
                float subz = (ogm_fusion_maxh[index] - ogm_fusion_minh[index]);

                for(int k =0; k<subz/param.ogm_resolution; ++k){
                    pcl::PointXYZ p;
                    p.x = (j + 0.5) * param.ogm_resolution - param.offset_x * param.ogm_resolution;
                    p.y = (i + 0.5) * param.ogm_resolution - param.offset_y * param.ogm_resolution;
                    p.z = (ogm_fusion_minh[index]) + k * param.ogm_resolution;
                    cloud_ob->points.push_back(p);
                }
            }
        }
    }

    // cout<<"cloud_ob->points. "<<cloud_ob->points.size()<<" "<<cloud_gr->points.size()<<"\n";
    std::cout<<"notify one getdata \n";
    std::unique_lock<std::mutex> lockk(mtx);
    data_ready = true;
    lockk.unlock();
    std::cout<<"data ready "<<data_ready.load()<<"\n";
    cvt.notify_all();
}

void TerrainSeg::GetData(std::vector<unsigned char>& ogm, 
                        pcl::PointCloud<pcl::PointXYZ>& cloud_ground, 
                        pcl::PointCloud<pcl::PointXYZ>& cloud_obstacle,
                        cv::Mat& range_img, cv::Mat& seg_img_out, cv::Mat& ogm_image){
    
    std::unique_lock<std::mutex> lock(mtx); // 加锁              
    cvt.wait(lock, [this](){  std::cout<<"GetData data ready "<<data_ready.load()<<"\n"; return data_ready.load(); });
    std::cout<<"receive data get data \n";
    timeval tm;
    gettimeofday(&tm ,NULL);
    std::cout<<"start "<<(uint64_t)(tm.tv_sec * 1e3 + tm.tv_usec * 1e-3)<<"\n";
    ogm.assign(ogm_fusion.begin(), ogm_fusion.end());
    cloud_ground.points.assign(cloud_gr->points.begin(), cloud_gr->points.end());
    cloud_obstacle.points.assign(cloud_ob->points.begin(), cloud_ob->points.end());
    Visualize(range_img, seg_img_out, ogm_image);
    gettimeofday(&tm ,NULL);
    std::cout<<"end "<<(uint64_t)(tm.tv_sec * 1e3 + tm.tv_usec * 1e-3)<<"\n";
    data_ready = false;
}

void calRangeDiff(const PointXYZIRT& p1, const PointXYZIRT& p2, float& range){
	range = sqrt((p1.x- p2.x)*(p1.x- p2.x)+(p1.y- p2.y)*(p1.y- p2.y) +(p1.z- p2.z)*(p1.z- p2.z));
}


void TerrainSeg::Visualize(cv::Mat& range_img, cv::Mat& seg_img_out, cv::Mat& ogm_image){
    cv::Mat img = cv::Mat::zeros(param.r_height, param.r_width, CV_8UC3);
    cv::Mat seg_img = cv::Mat::zeros(param.r_height, param.r_width, CV_8UC3);

    pcl::PointCloud<PointXYZIRT>::Ptr obstacle_cloud( new pcl::PointCloud<PointXYZIRT>);
    pcl::PointCloud<PointXYZIRT>::Ptr ground_cloud( new pcl::PointCloud<PointXYZIRT>);

    if(range_map.range.size() != param.r_height * param.r_width)
        return;

    for(int i=0; i<param.r_height; ++i){
        for(int j =0; j<param.r_width; ++j){
            float range = range_map.range[i*param.r_width + j];
            int gray = (range/70 * 200 + 50);

            img.at<cv::Vec3b>(i,j) = cv::Vec3b(gray,gray,gray);

            if (range_map.regionID[i * param.r_width + j]==EDGEPT) {
                seg_img.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 255, 0);
            }
            else if (range_map.regionID[i * param.r_width + j]==NONVALID)
            {
                seg_img.at<cv::Vec3b>(i,j) = cv::Vec3b( 0, 0, 0);
            }
            else if (range_map.regionID[i * param.r_width + j]==UNKNOWN)
            {
                seg_img.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 255, 0);
            }
            else {
                SEGBUF *segbuf = &range_map.segbuf[range_map.regionID[i * param.r_width + j]];
                if (segbuf->ptnum > 0) {
                    // setPixel(rm.lMap, y, x, nint(fabs(segbuf->norm.x)*255.0), nint(fabs(segbuf->norm.y)*255.0), nint(segbuf->norm.z*255.0));
                    seg_img.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 0, 255);
                    ground_cloud->points.push_back(range_map.pts[i * param.r_width + j]);
                }
            }
        }
    }

	// vector<cv::Mat> channels;
	// cv::split(seg_img, channels);
	// cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
	// cv::dilate(channels[2],channels[2],element);
	// cv::merge(channels, seg_img);
	// queue<std::pair<int,int> > qt;
	// for(int i=0; i<param.r_height; ++i){
	// 	for(int j= 0; j<param.r_width; ++j){
	// 		if(seg_img.at<cv::Vec3b>(i,j) == cv::Vec3b(0,255,255)){
    //             qt.push(std::pair(i,j));
	// 			if(range_map.idx[i * param.r_width + j] != -1){
	// 				seg_img.at<cv::Vec3b>(i,j) = cv::Vec3b(255,0,0);
	// 			}else{
	// 				seg_img.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,255);
	// 			}
	// 		}
	// 	}
	// }

    // while(!qt.empty()){
	// 	std::pair<int,int> id = qt.front();
	// 	qt.pop();
	// 	int cloud_id = id.first * param.r_width + id.second;
	// 	Eigen::VectorXf D(24);
	// 	int mask[24];
	// 	float sumD(0);
	// 	for(int i=0; i<24; ++i){
	// 		int nx = neighborx_[i] + id.first;
	// 		int ny = neighbory_[i] + id.second;

	// 		int ncloud_id = nx * param.r_width + ny;
	// 		float range_diff(0);
			
	// 		if(nx < 0 || nx >= param.r_width || ny < 0 ||ny >= param.r_height || range_map.idx[ncloud_id] == -1){
	// 			D(i) = 0;
	// 			sumD += D(i) ;
	// 			mask[i] = -1;		
	// 			continue;

	// 		}

	// 		calRangeDiff(range_map.pts[cloud_id], range_map.pts[ncloud_id], range_diff);
	// 		if(range_diff > 3){
	// 			D(i) = 0;
	// 			sumD += D(i);
	// 		}else{
	// 			D(i) = (exp(-5 * range_diff));
	// 			sumD += D(i);
	// 		}
	// 		if(seg_img.at<cv::Vec3b>(ny, nx) == cv::Vec3b(255,0,0)){
	// 			mask[i] = 2;
	// 		}else if(seg_img.at<cv::Vec3b>(ny, nx) == cv::Vec3b(0,255,0)){
	// 			mask[i] = 1;
	// 		}else if(seg_img.at<cv::Vec3b>(ny, nx) == cv::Vec3b(0,0,255)){
	// 			mask[i] = 0;
	// 		}
	// 	}

	// 	Eigen::VectorXf W(24);
	// 	W = D / sumD;
	// 	float score_r(0), score_g(0);
	// 	for(int i=0; i<D.size(); ++i){
	// 		if(mask[i] == 0){
	// 			score_r += W(i);
	// 		}else if(mask[i] == 1){
	// 			score_g += W(i);
	// 		}
	// 	}
		
	// 	if(score_r > score_g){
	// 		seg_img.at<cv::Vec3b>(id.first, id.second) = cv::Vec3b(0,0,255);
	// 	}else{
	// 		seg_img.at<cv::Vec3b>(id.first, id.second) = cv::Vec3b(0,255,0);
	// 	}

	// }

    cv::resize(img,img, cv::Size((int)1000, (int)300));
    cv::resize(seg_img,seg_img, cv::Size((int)1000, (int)300));

    cv::flip(img,img,0);
    cv::flip(img,img,1);

    cv::flip(seg_img,seg_img,0);
    cv::flip(seg_img,seg_img,1);

    // cv::imshow("range", img);
    // cv::imshow("seg_img", seg_img);

    cv::Mat ogm_img = cv::Mat::zeros(param.ogm_height, param.ogm_width, CV_8UC3);
    for(int i=0; i<param.ogm_height; ++i){
        for(int j =0; j<param.ogm_width; ++j){
            if(ogm_map.lab[i*param.ogm_width + j] == TRAVESABLE){
                ogm_img.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 240, 0);
            }
            else if(ogm_map.lab[i*param.ogm_width + j] == NONTRAVESABLE){
                ogm_img.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 0, 255);
            }
        }
    }

    // cv::imshow("ogm_img", ogm_img);
    // cv::waitKey(0);

    img.copyTo(range_img);
    seg_img.copyTo(seg_img_out);
    ogm_img.copyTo(ogm_image);

    // boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer);
    // viewer->setBackgroundColor(0.1, 0.1, 0.1);
    // viewer->addCoordinateSystem(1);
    // pcl::visualization::PointCloudColorHandlerCustom <PointXYZIRT> colorob(cloud_g, (255), (0),(0));
    // std::string csob = "cloudfinalob";
    // viewer->addPointCloud(cloud_g,colorob, csob);

    // pcl::visualization::PointCloudColorHandlerCustom <PointXYZIRT> colorgr(ground_cloud, (0), (255),(0));
    // std::string clg = "cloudground";
    // viewer->addPointCloud(ground_cloud, colorgr, clg);

    // while(!viewer->wasStopped()){
	// 	viewer->spinOnce(10);
    // }
}