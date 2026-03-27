#include "terrain_seg.h"
using namespace std;

static float CalPolarAnglex(float x, float y){
    float temp_tangle(0);
    if (x == 0 && y == 0) {
        return 0.0f;
    }
    float angle = static_cast<float>(atan2(y, x));
    return (angle < 0) ? (angle + 2 * M_PI) : angle;
}


static double DistancePP (PointXYZIRT *pt1, PointXYZIRT *pt2)
{
    const double dx = pt1->x - pt2->x;
    const double dy = pt1->y - pt2->y;
    const double dz = pt1->z - pt2->z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

static double p2r (PointXYZIRT *pt1)
{
    return (std::sqrt(pow(pt1->x ,2 )+pow(pt1->y ,2 )+pow(pt1->z ,2 )));
}

void TerrainSeg::GenerateRangeMap(){

    int count = -1;
    for(auto point:cloud_g->points){
        count++;

        float range = std::sqrt(point.x*point.x + point.y*point.y);;
        float angle = CalPolarAnglex(point.x, point.y);

        int px = round((angle*180/M_PI - param.min_angle)/param.w_res);
        int py = point.ring;

        if(px >= 0 && px < param.r_width && py >= 0 && py < param.r_height){
            int rindex = point.ring * param.r_width + px;
            auto& idx = range_map.idx[rindex];
            auto& range_val = range_map.range[rindex];
            if (idx == -1 || range < range_val) {
                idx = count;
                range_val = range;
                range_map.pts[rindex] = point;
            }
        }
    }
}

//对 range image 进行 平滑操作
void TerrainSeg::SmoothMap(){
    int maxcnt = 3;
    for (int y=0; y<param.r_height; y++) {
        for (int x=1; x<(param.r_width-1); x++) {
            //横向持续找连续空的像素点
            int row_base = y*param.r_width;

            if (range_map.idx[row_base+(x-1)] != -1 && range_map.idx[y*param.r_width+x] == -1) {

                int xx;
                for (xx = x+1; xx<param.r_width; xx++) {
                    if (range_map.idx[row_base+xx] != -1) 
                        break;
                }
                if (xx >= param.r_width) 
                    continue;

                int cnt = xx-x+1;
                if (cnt > maxcnt) {
                    x = xx;
                    continue;
                }

                // 有连续不超过maxcnt个无效像素点，补齐
                PointXYZIRT *p1 = &range_map.pts[row_base + (x-1)];
                PointXYZIRT *p2 = &range_map.pts[row_base + xx];
                double dis = DistancePP (p1, p2);
                double rng = std::max(p2r(p1),p2r(p2));

                double maxdis = std::min(param.max_smooth_error, 
                                         std::max(param.base_smooth_error, param.smooth_hori_factor*cnt*rng));
                if (dis<maxdis) {
                    for (int xxx = x; xxx < xx; xxx++) {
                        PointXYZIRT *p = &range_map.pts[y*param.r_width+xxx];

                        //线性插值
                        p->x = (p2->x-p1->x)/cnt * (xxx-x+1) + p1->x;
                        p->y = (p2->y-p1->y)/cnt * (xxx-x+1) + p1->y;
                        p->z = (p2->z-p1->z)/cnt * (xxx-x+1) + p1->z;
                        p->intensity = p1->intensity;
                        p->ring = p->ring;
                    
                        range_map.idx[y*param.r_width+xxx] = -2;
                        range_map.range[y*param.r_width+xxx] = sqrt(pow(p->x,2) + pow(p->y,2));
                    }
                }
                x = xx;
            }
        }
    }
}

// 边界分割
int TerrainSeg::ContourSegger()
{
    // 标注边界点
    ContourExtraction();
    // number of ground seg region
    range_map.regnum = RegionGrow()+1;

    return true;
}

// 判断相邻两点是否为同一个物体 平面或障碍物
bool TerrainSeg::isppNeighbor(PointXYZIRT *pt, PointXYZIRT *cp, bool ishori)
{
    double rng = std::max (p2r(pt),p2r(cp));

    //依据是否是水平还是垂直方向选取阈值

    float dmax = (rng) * sin(param.w_res*M_PI/180)/sin(20.*M_PI/180 - param.w_res*M_PI/180) 
                    + 3*0.15;

    double maxdd = ishori?dmax
                            :std::max(param.base_smooth_error,0.35*rng);
    double dist = DistancePP (pt, cp);

    // 距离过远就不判断了
    if (dist > maxdd) 
        return false;

    bool isneighbor = false;
    double dz = fabs(pt->z - cp->z);
    double horizonDis = sqrt(pow(pt->x, 2)+ pow(pt->y, 2));
    double pp_neighbour_height_limit(0);
    double dh = sqrt(pow(pt->x-cp->x, 2) + pow(pt->y-cp->y, 2));
    double ang = atan2(dz, dh);

    // 远处激光线较稀疏，远近用不同的阈值
    if (horizonDis < param.far_range) 
        pp_neighbour_height_limit = param.pp_height_thresh;      // 近处的阈值
    else 
        pp_neighbour_height_limit = param.pp_height_thresh_far;  // 远处的阈值

    // 高度差小
    if (dz < pp_neighbour_height_limit) {
        // 需要避免竖着的整个墙面被RegionGrow
        // 计算两个点的水平距离
        if (ang > param.pp_vertical_angle_thresh)      // 71 degree 几乎竖直的墙面
            isneighbor = false;
        else
            isneighbor = true;
    }
    // 高度差大
    else {
        // 高度差大，水平距离小 --> 比较陡  (可以直接用坡度判断，所以把这个if注释掉)
        // if (dh < conf.PP_NEIGHBOUR_HORIZON_LIMIT)      // 0.56
        //     isneighbor = false;
        // else {
        //     高度差较大，判断【坡度】
        if (ang < param.pp_angle_thresh)		//0.17  5deg
            isneighbor = true;
        else
            isneighbor = false;
        // }
    }
    return isneighbor;
}


bool TerrainSeg::AddPoints(point2d seed, std::vector<point2d> & vec, int _regid)
{
    if (range_map.regionID[param.r_width*seed.x +seed.y] == UNKNOWN)
        range_map.regionID[param.r_width*seed.x +seed.y] = _regid;
    // 如果输入的seed不是当前类，不加
    else if (range_map.regionID[param.r_width*seed.x +seed.y] != _regid)
        return false;

    point2d Neighbor;

    PointXYZIRT *cp = &range_map.pts[param.r_width*seed.x +seed.y];

    for (int k=0; k<4; k++)
    {
        double errfactor;
        switch (k) {
        case 0:	Neighbor.x = seed.x - 1; Neighbor.y = seed.y;     errfactor = param.vertifactor; break;
        case 1: Neighbor.x = seed.x + 1; Neighbor.y = seed.y;     errfactor = param.vertifactor; break;
        case 2: Neighbor.x = seed.x;     Neighbor.y = seed.y - 1; errfactor = param.horizfactor; break;
        case 3: Neighbor.x = seed.x;     Neighbor.y = seed.y + 1; errfactor = param.horizfactor; break;
		// case 4:	Neighbor.x = seed.x - 1; Neighbor.y = seed.y - 1;errfactor=(VERTERRFACTOR+HORIERRFACTOR)/2.0; break;
		// case 5: Neighbor.x = seed.x + 1; Neighbor.y = seed.y + 1;errfactor=(VERTERRFACTOR+HORIERRFACTOR)/2.0; break;
		// case 6: Neighbor.x = seed.x + 1; Neighbor.y = seed.y - 1;errfactor=(VERTERRFACTOR+HORIERRFACTOR)/2.0; break;
		// case 7: Neighbor.x = seed.x - 1; Neighbor.y = seed.y + 1;errfactor=(VERTERRFACTOR+HORIERRFACTOR)/2.0; break;
        }

        if (Neighbor.x >= 0 && Neighbor.x < param.r_height &&
            Neighbor.y >= 0 && Neighbor.y < param.r_width &&
            range_map.idx[param.r_width*Neighbor.x + Neighbor.y] != -1 &&
            range_map.regionID[param.r_width*Neighbor.x +Neighbor.y] == UNKNOWN)
        {
            vec.push_back(Neighbor);
            range_map.regionID[param.r_width*Neighbor.x +Neighbor.y] = _regid;
        }
    }
    return true;
}

void TerrainSeg::ContourExtraction ()
{
    PointXYZIRT *cp,*pt;
    int x,y,xx,yy;

    for (x = 0; x < param.r_height; x++) {
        for (y = 0; y < param.r_width; y++) {
            cp = &range_map.pts[param.r_width*x+y];

            if (range_map.idx[param.r_width*x+y] == -1) {
                range_map.regionID[param.r_width*x +y] = NONVALID;
                continue;
            }

            // 遍历4联通区间
            for (yy = y-1; yy <= y+1; yy++) {
                if(yy < 0 || yy >= param.r_width)
                    continue;
                for (xx = x-1; xx <= x+1; xx++) {
                    if (xx<0||xx>=param.r_height) 
                        continue;
                    
                    if (xx!=x && yy!=y) 
                        continue;

                    pt = &range_map.pts[param.r_width*xx + yy];
                    if (range_map.idx[param.r_width*xx + yy] == -1) 
                        continue;

                    // 判断是否是左右两个像素
                    bool ishori = (yy == y) ? false : true;
                    if (!isppNeighbor (pt, cp, ishori)) {
                        range_map.regionID[param.r_width*x + y] = EDGEPT;
                        break;
                    }
                }
                //提前结束遍历
                if (range_map.regionID[param.r_width*x + y] == EDGEPT)
                    break;
            }
        }
    }
}

void TerrainSeg::GrowOne(point2d seed, unsigned int _regid)
{
    std::vector<point2d> curr_vec, next_vec;
    curr_vec.push_back(seed);

    while (!curr_vec.empty()) {
        next_vec.clear();
        const size_t vec_size = curr_vec.size();
        for (size_t j = 0; j < vec_size; j++) {
            AddPoints(curr_vec[j], next_vec, _regid);
        }
        // 交换容器，避免拷贝
        curr_vec.swap(next_vec);
    }
}

unsigned int TerrainSeg::RegionGrow()
{
    point2d	seed;
    int m,x1,x2;
    unsigned int _regid = 1;

    int x,y,yf;
    for (x=0; x < param.r_height; x++) {
        for (yf = 0; yf < param.r_width; yf++) {
            if (range_map.idx[param.r_width*x+yf] != -1) 
                break;
        }
        if(yf >= param.r_width) continue;
        for (y = yf; y<param.r_width; y++)
        {
            // 如果是未标记过的点，且在路面高度范围内
            if (range_map.regionID[param.r_width*x + y] == UNKNOWN 
                && range_map.pts[param.r_width*x + y].z>param.road_lowerbound 
                && range_map.pts[param.r_width*x + y].z<param.road_upperbound) {

                seed.x = x;
                seed.y = y;
                // 区域增长
                GrowOne(seed, _regid);
                _regid++;
            }
        }
    }
    return _regid-1;
}


void TerrainSeg::Region2Seg ()
{
    SEGBUF *segbuf;
    int		regionid;

    for (regionid = 0; regionid < range_map.regnum; regionid++) {
        segbuf = &range_map.segbuf[regionid];
        segbuf->minxp.x = segbuf->minxp.y = segbuf->minxp.z = 9999.0;
        segbuf->minyp.x = segbuf->minyp.y = segbuf->minyp.z = 9999.0;
        segbuf->minzp.x = segbuf->minzp.y = segbuf->minzp.z = 9999.0;
        segbuf->maxxp.x = segbuf->maxxp.y =	segbuf->maxxp.z = -9999.0;
        segbuf->maxyp.x = segbuf->maxyp.y =	segbuf->maxyp.z = -9999.0;
        segbuf->maxzp.x = segbuf->maxzp.y =	segbuf->maxzp.z = -9999.0;
        segbuf->cp.x = segbuf->cp.y = segbuf->cp.z = 0;
        segbuf->dmin.x = param.r_width;
        segbuf->dmin.y = param.r_height;
        segbuf->ptnum = 0;
    }

    //为每个regionID，生成一个segbuf，记录该区域块的特征
    int x, y;
    for (y=0; y<param.r_height; y++) {
        for (x=0; x<param.r_width; x++) {
            if (range_map.idx[y*param.r_width+x]==-1)
                continue;
            regionid = range_map.regionID[y*param.r_width+x];
            if (regionid <= 0 || regionid >= range_map.regnum)
                continue;

            segbuf = &range_map.segbuf[regionid];
            // center point sum
            segbuf->cp.x += range_map.pts[y*param.r_width+x].x;
            segbuf->cp.y += range_map.pts[y*param.r_width+x].y;
            segbuf->cp.z += range_map.pts[y*param.r_width+x].z;
            // min/max x/y boundary
            segbuf->dmin.x = min (segbuf->dmin.x, x);
            segbuf->dmin.y = min (segbuf->dmin.y, y);
            segbuf->dmax.x = max (segbuf->dmax.x, x);
            segbuf->dmax.y = max (segbuf->dmax.y, y);
            // min/max(x,y,z) point feature
            if (segbuf->minxp.x>range_map.pts[y*param.r_width+x].x)
                segbuf->minxp = range_map.pts[y*param.r_width+x];
            if (segbuf->minyp.y>range_map.pts[y*param.r_width+x].y)
                segbuf->minyp = range_map.pts[y*param.r_width+x];
            if (segbuf->minzp.z>range_map.pts[y*param.r_width+x].z)
                segbuf->minzp = range_map.pts[y*param.r_width+x];
            if (segbuf->maxxp.x<range_map.pts[y*param.r_width+x].x)
                segbuf->maxxp = range_map.pts[y*param.r_width+x];
            if (segbuf->maxyp.y<range_map.pts[y*param.r_width+x].y)
                segbuf->maxyp = range_map.pts[y*param.r_width+x];
            if (segbuf->maxzp.z<range_map.pts[y*param.r_width+x].z)
                segbuf->maxzp = range_map.pts[y*param.r_width+x];
            segbuf->ptnum++;
        }
    }

    double Equation[4]; //A,B,C,D
    double WX[6],WY[6],WZ[6];
    int num;
    for (regionid=0; regionid<range_map.regnum; regionid++) {
        segbuf = &range_map.segbuf[regionid];
        if (segbuf->ptnum < param.road_region_min_point) {
            segbuf->ptnum = 0;
            continue;
        }
        // center point
        segbuf->cp.x /= (double)segbuf->ptnum;
        segbuf->cp.y /= (double)segbuf->ptnum;
        segbuf->cp.z /= (double)segbuf->ptnum;

        WX[0]=segbuf->minxp.x;WX[1]=segbuf->minyp.x;WX[2]=segbuf->minzp.x;WX[3]=segbuf->maxxp.x;WX[4]=segbuf->maxyp.x;WX[5]=segbuf->maxzp.x;
        WY[0]=segbuf->minxp.y;WY[1]=segbuf->minyp.y;WY[2]=segbuf->minzp.y;WY[3]=segbuf->maxxp.y;WY[4]=segbuf->maxyp.y;WY[5]=segbuf->maxzp.y;
        WZ[0]=segbuf->minxp.z;WZ[1]=segbuf->minyp.z;WZ[2]=segbuf->minzp.z;WZ[3]=segbuf->maxxp.z;WZ[4]=segbuf->maxyp.z;WZ[5]=segbuf->maxzp.z;
        num = 6;
        Calculate_Plane(num,WX,WY,WZ,0,Equation);
        Calculate_Residuals(&segbuf->cp.x,&segbuf->cp.y,&segbuf->cp.z,Equation,&segbuf->var,1);// total residual
        //这里为什么要变负数？
        
        if (Equation[2]<0) {
            segbuf->norm.x = -Equation[0];
            segbuf->norm.y = -Equation[1];
            segbuf->norm.z = -Equation[2];
        }
        else {
            segbuf->norm.x = Equation[0];
            segbuf->norm.y = Equation[1];
            segbuf->norm.z = Equation[2];
        }
        //这里判断是否为平面
        // cout<<segbuf->norm.z<<"\n";
        if (segbuf->norm.z < param.seg_plane_znorm) {
            segbuf->ptnum = 0;
        }
        else
            segbuf->ptnum=segbuf->ptnum;
    }
}