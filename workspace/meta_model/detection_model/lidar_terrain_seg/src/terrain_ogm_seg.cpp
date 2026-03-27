#include "terrain_seg.h"

using namespace std;

void TerrainSeg::GenerateLocOgm(){
    ogm_map.demg.clear();
    ogm_map.demgnum.clear();
    ogm_map.demhmin.clear();
    ogm_map.demhmax.clear();
    ogm_map.demhnum.clear();
    ogm_map.lab.clear();

    ogm_map.demg.resize(param.ogm_width * param.ogm_height, 0);
    ogm_map.demgnum.resize(param.ogm_width * param.ogm_height, 0);
    ogm_map.demhmin.resize(param.ogm_width * param.ogm_height, 10e6);
    ogm_map.demhmax.resize(param.ogm_width * param.ogm_height, -10e6);
    ogm_map.demhnum.resize(param.ogm_width * param.ogm_height, 0);
    ogm_map.lab.resize(param.ogm_width * param.ogm_height, UNKNOWN);

    for (int ry=0; ry < param.r_height; ry++) {
        for (int rx=0; rx < param.r_width; rx++) {
            if (range_map.idx[ry * param.r_width + rx] == -1)
                continue;
            PointXYZIRT *p = &range_map.pts[ry * param.r_width+rx];
            // 处理路面区域
            bool isroad;
            if (range_map.regionID[ry * param.r_width + rx] <= 0 
                || range_map.regionID[ry*param.r_width + rx] > range_map.regnum){
                isroad=false;
            }else {
                // 区域太小认为不是路面,（点数太少，这里说区域小可能不太合适，因为远处点稀疏）
                SEGBUF *segbuf = &range_map.segbuf[range_map.regionID[ry*param.r_width+rx]];
                if (segbuf->ptnum > param.road_region_min_point)
                    isroad=true;
                else
                    isroad=false;
            }
            float ix, iy; 
            ix = round(p->x/param.ogm_resolution) + param.offset_x;
            iy = round(p->y/param.ogm_resolution) + param.offset_y;

            if(ix <0 || ix >= param.ogm_width || iy < 0 || iy >= param.ogm_height)
                continue;

            if (isroad) {
                ogm_map.demg[iy*param.ogm_width + ix] += p->z;
                ogm_map.demgnum[iy*param.ogm_width+ix]++;
            }

            ogm_map.demhmin[iy*param.ogm_width+ix] = min (ogm_map.demhmin[iy*param.ogm_width+ix],(double)p->z);
            ogm_map.demhmax[iy*param.ogm_width+ix] = max (ogm_map.demhmax[iy*param.ogm_width+ix],(double)p->z);
            ogm_map.demhnum[iy*param.ogm_width+ix] += 1;
        }
    }

    cv::Mat dem_height = cv::Mat::zeros(cv::Size(param.ogm_width, param.ogm_height), CV_8UC1);
    float minzh = -2;
    float maxzh = 3;

    for (int y=0; y<param.ogm_height; y++) {
        for (int x=0; x<param.ogm_width; x++) {
            if(ogm_map.demgnum[y*param.ogm_width+x] != 0){
                ogm_map.demg[y*param.ogm_width+x] /= (double)ogm_map.demgnum[y*param.ogm_width+x];
                dem_height.at<uchar>(y,x) = (ogm_map.demg[y*param.ogm_width+x] - minzh) / (maxzh - minzh) * 255.;
            }else{
                ogm_map.demg[y*param.ogm_width+x] = 1e6;
            }
        }
    }

    cv::dilate(dem_height, dem_height, dilate_element_);  //膨胀
    cv::filter2D(dem_height, dem_height, -1, mean_kernel, cv::Point2i(-1,-1),0.0,cv::BORDER_REPLICATE);

    for (int y=0; y<param.ogm_height; y++) {
        for (int x=0; x<param.ogm_width; x++) {
            if(dem_height.at<uchar>(y,x) != 0){
                ogm_map.demg[y*param.ogm_width+x] = (float)dem_height.at<uchar>(y,x) / 255. *(maxzh - minzh) + minzh; 
            }
        }
    }

    for(int k=0; k<vehicle_around.size(); ++k){
        ogm_map.demg[vehicle_around[k].first*param.ogm_width + vehicle_around[k].second] = 0;
    }

    dem_height = cv::Mat::zeros(cv::Size(param.ogm_width, param.ogm_height), CV_8UC1);
    // std::vector<double> minz_area_ogm(param.ogm_height * param.ogm_width, 1e6);
    // for (int y=0; y<param.ogm_height; y++) {
    //     for (int x=0; x<param.ogm_width; x++){
    //         int cur_index = y * param.ogm_width + x;
    //         for(int k=0; k<24; ++k){
    //             int nx = x + neighborx_[k];
    //             int ny = y + neighbory_[k];
    //             if(nx<0 || nx >= param.ogm_width || ny<0 || ny>=param.ogm_height)
    //                 continue;
    //             int index = ny * param.ogm_width + nx;
    //             minz_area_ogm[cur_index] = min(minz_area_ogm[cur_index], ogm_map.demg[index]);
    //         }
    //     }
    // }
    // ogm_map.demg.assign(minz_area_ogm.begin(), minz_area_ogm.end());

    for(int y=0; y<param.ogm_height; y++){
        for (int x=0; x<param.ogm_width; x++) {
            // 没有点跳过
            if(!ogm_map.demhnum[y*param.ogm_width+x]){
                ogm_map.lab[y*param.ogm_width+x] = UNKNOWN;
                continue;
            }else{
                //只有平面
                if(ogm_map.demgnum[y*param.ogm_width+x] == ogm_map.demhnum[y*param.ogm_width+x]){
                    // 如果全是地面的话，考虑格子里面的高度差，要是高度差大，则不是地面
                    double diff_h = ogm_map.demhmax[y*param.ogm_width+x] - ogm_map.demhmin[y*param.ogm_width+x];
                    if(diff_h > 0.2){
                        ogm_map.lab[y*param.ogm_width+x] = NONTRAVESABLE;
                        dem_height.at<uchar>(y,x) = 255;

                    }else{
                        ogm_map.lab[y*param.ogm_width+x] = TRAVESABLE;
                    } 
                }else{
                    // 如果有地面高程
                    if(ogm_map.demg[y*param.ogm_width+x] != 1e6){
                        if(ogm_map.demhmax[y*param.ogm_width + x] <= ogm_map.demg[y*param.ogm_width+x] + param.pos_minheight){
                            // 高度差再判断
                            if(ogm_map.demhmax[y*param.ogm_width + x] - ogm_map.demhmin[y*param.ogm_width + x] < param.pos_minheight)
                                ogm_map.lab[y*param.ogm_width+x] = TRAVESABLE;
                            else{
                                ogm_map.lab[y*param.ogm_width+x] = NONTRAVESABLE;
                                dem_height.at<uchar>(y,x) = 255;
                            }
                        //悬空障碍物
                        }else if((ogm_map.demhmin[y*param.ogm_width + x] - ogm_map.demg[y*param.ogm_width+x]) > param.vehicle_height){

                            ogm_map.lab[y*param.ogm_width+x] = TRAVESABLE;

                        }else{
                            ogm_map.lab[y*param.ogm_width+x] = NONTRAVESABLE;
                            dem_height.at<uchar>(y,x) = 255;
                        }
                    }
                    else{
                        ogm_map.lab[y*param.ogm_width+x] = NONTRAVESABLE;
                        dem_height.at<uchar>(y,x) = 255;
                    }
                }
            }
        }
    }   
    
    cv::dilate(dem_height,dem_height,dilate_element_);  //膨胀

    cv::Mat stats;
    cv::Mat labels;
    cv::Mat centroids;
    int i, nccomps = cv::connectedComponentsWithStats(dem_height, labels, stats, centroids);
    std::vector<int> cluster(nccomps, 0);

    std::vector<std::pair<int,int> > label_centroid;
    label_centroid.resize(nccomps);

    for(int y=0; y<param.ogm_height; y++){
        for (int x=0; x<param.ogm_width; x++) {
            int index = y * param.ogm_width + x;
            if(ogm_map.demhnum[index] <= 0)
                continue;
            int label = labels.at<int>(y, x);
            if(!(0 <= label && label <= nccomps)){
                continue;
            }
            cluster[label] += ogm_map.demhnum[index]; 
            label_centroid[label] = std::pair<int,int>(centroids.at<double>(label,0),centroids.at<double>(label,1));
        }
    }

    for(int y=0; y<param.ogm_height; y++){
        for (int x=0; x<param.ogm_width; x++) {
            int index = y * param.ogm_width + x;
            if(ogm_map.demhnum[index] <= 0)
                continue;
            int label = labels.at<int>(y, x);
            if(!(0 <= label && label <= nccomps)){
                continue;
            }
            // 根据距离调整障碍物阈值大小
            if(fabs(label_centroid[label].second - param.offset_y) > (param.far_range/param.ogm_resolution)){
                continue;
            }else if(cluster[label] <= 5){
                ogm_map.lab[index] = TRAVESABLE;
            }
        }
    }
}