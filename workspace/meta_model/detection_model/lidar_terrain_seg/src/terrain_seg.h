#ifndef TERRAIN_SEG
#define TERRAIN_SEG
#include "common.h"
#include "map_fusion.h"
#include "../perception_common/datatype/common_datatype.h"
#include "../perception_common/utm/utm.h"
#include <condition_variable>
class TerrainSeg{
public:
    TerrainSeg();
    ~TerrainSeg();

    void LoadConfig(std::string& config_path);
    void Init();

    template <typename PointType>
    void InputPointcloud(pcl::PointCloud<PointType>& cloud_in);
    void InputPosition(percep_datatype::Position pos);

    void RangeSegProcess();
    
    void OgmProcess();

    void Process();

    void Visualize(cv::Mat& range_img, cv::Mat& seg_img_out, cv::Mat& ogm_image);

    void GetData(std::vector<unsigned char>& ogm, 
                    pcl::PointCloud<pcl::PointXYZ>& cloud_ground, 
                    pcl::PointCloud<pcl::PointXYZ>& cloud_obstacle,
                    cv::Mat& range_img, cv::Mat& seg_img_out, cv::Mat& ogm_image);
    void GetOgmInfo(std::vector<float>& param_out);
private:

    // range seg
    void GenerateRangeMap();
    void SmoothMap();

    int ContourSegger();
    void ContourExtraction();
    int  FindFirstValidPt(int scanno);
    void calcRectOpti();
    void Region2Seg ();
    bool isppNeighbor (PointXYZIRT *pt, PointXYZIRT *cp, bool ishori);

    bool AddPoints(point2d seed, std::vector<point2d> & vec, int _regid);
    unsigned int RegionGrow();
    void GrowOne(point2d seed, unsigned int _regid);

    // ogm generate
    void GenerateLocOgm();

    RMAP range_map;
    DMAP ogm_map;

    ConfigParam param;

    int neighborx_[24] = {-2, -1,  0,  1,  2,-2, -1,  0,  1,  2,-2,-1, 1, 2, -2,-1,0, 1, 2,-2,-1, 0, 1, 2};
    int neighbory_[24] = {-2, -2, -2, -2, -2,-1, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
    std::unique_ptr<MapFusion> mapfusion_;

    pcl::PointCloud<PointXYZIRT>::Ptr cloud_input;
    pcl::PointCloud<PointXYZIRT>::Ptr cloud_g;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_ob;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_gr;
    percep_datatype::Position pos_global;
    std::vector<unsigned char> ogm_fusion;

    int ogm_kernel_num = 5;

    int  ogm_cell_size_;

    std::atomic<bool>  data_ready{false};
    std::atomic<bool>  data_receive{false};
    std::mutex mtx; 
    std::mutex lidar_mtx; 

    std::condition_variable cvt;
    std::condition_variable ldt;

    std::thread* process_thread;

    std::atomic<bool>  stop_flag{false};

    std::vector<std::pair<int, int> > vehicle_around;
    cv::Mat dilate_element_;
    cv::Mat mean_kernel;
};

template <typename PointType>
void TerrainSeg::InputPointcloud(pcl::PointCloud<PointType>& cloud_in){
    cloud_input->points.clear();
    for(auto &p:cloud_in.points){
        PointXYZIRT pin;
        pin.x = p.x;
        pin.y = p.y;
        pin.z = p.z;
        pin.intensity = p.intensity;
        pin.ring = p.ring;
        cloud_input->points.push_back(pin);
    }
    std::cout<<"############\n";
    std::unique_lock<std::mutex> lock(lidar_mtx);
    data_receive = true;
    lock.unlock();
    ldt.notify_one();
}    


#endif