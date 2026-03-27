#ifndef SPEEDPLAN_HEADER_H_
#define SPEEDPLAN_HEADER_H_
// #include "param_reader.h"
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/datapool/perception_struct.h"
#include <iostream>
#include <algorithm>
// using namespace avos::planning;
namespace avos {
namespace motionplan {

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif
  struct sPoint {
    float length;
    geometry::Site pathpoints_in;
    sPoint(){
      Reset();
    }
    void Reset(){
      length = 0.0;
      pathpoints_in.Reset();
    }
  };
  struct pt
  {
    double x;
    double y;
    pt() {
      x = 0.0;
      y = 0.0;
    }
    void reset() {
      x = 0.0;
      y = 0.0;
    }
  };
  typedef struct sObjFocus
  {
    int posid;
    int roi_id;
    int roi_id_pre;
    unsigned int center_posid;
    int yeild_count;
    double s;   // s of obs center
    double l;   // l of obs center
    double objLonDist;
    double objLatDist;
    double angle;
    // double vx;
    // double vy;
    // double speed;
    avos::planning::Object objfocus;
    sObjFocus() {
      posid = 0;
      roi_id = 0;
      roi_id_pre = 0;
      center_posid = 0;
      yeild_count = 0;
      s = 0.0;   // s of obs center
      l = 0.0;   // l of obs center
      objLonDist = 0.0;
      objLatDist = 0.0;
      angle = 0.0;
    }
  } sObjSelected;
  typedef struct sObjPos
  {
    int posid;
    double dist;
    double length;
  } sObjPos;
  typedef struct objInformation
  {
    int objID;
    int yeild_count;
    int latDistLost_cnt;
    double latDist;
    double lonDist;
    objInformation() {
      objID = 0;
      yeild_count = 0;
      latDistLost_cnt = 0;
      latDist = 0.0;
      lonDist = 0.0;
    }
  } sObjInfo;
  enum eObjType   //TBD
  {
    ROI_INIT = 0,
    ROI_FRONT,
    ROI_BESIDE,
    ROI_NEAR,
    // ROI_FRONT_PRE,
    // ROI_BESIDE_PRE,
    // ROI_NEAR_PRE,
    ROI_PREDICT_STOP,
    ROI_PREDICT_SLOW
  };
  struct sObjPosInfo
  {
    int roi_id_pre;
    int Front_posid;
    int Beside_posid;
    unsigned int center_posid;
    double overlap_time;
    double s;
    double l;
    double angle;
    double dist;
    double Front_dist;
    double Beside_dist;
    double FrontLength;
    double BesideLength;
    sObjPosInfo() {
      roi_id_pre = ROI_INIT;
      Front_posid = 0;
      Beside_posid = 0;
      center_posid = 0;
      overlap_time = 0.0;
      s = 88888.0;
      l = 88888.0;
      angle = 0.0;
      dist = 88888.0;
      Front_dist = 88888.0;
      Beside_dist = 88888.0;
      FrontLength = 88888.0;
      BesideLength = 88888.0;
    }
    void reset() {
      roi_id_pre = ROI_INIT;
      Front_posid = 0;
      Beside_posid = 0;
      // center_posid = 0;
      overlap_time = 0.0;
      // s = 88888.0;
      // l = 88888.0;
      angle = 0.0;
      dist = 88888.0;
      Front_dist = 88888.0;
      Beside_dist = 88888.0;
      FrontLength = 88888.0;
      BesideLength = 88888.0;
    }
    void reset_all() {
      roi_id_pre = ROI_INIT;
      Front_posid = 0;
      Beside_posid = 0;
      center_posid = 0;
      overlap_time = 0.0;
      s = 88888.0;
      l = 88888.0;
      angle = 0.0;
      dist = 88888.0;
      Front_dist = 88888.0;
      Beside_dist = 88888.0;
      FrontLength = 88888.0;
      BesideLength = 88888.0;
    }
  };
  enum ePathDirection
  {
    Drive = 0,
    ShortReverse ,
    Reverse
  };
  enum eShiftPosition
  {
    P_ = 0,
    R_ ,
    N_ ,
    D_
  };
  enum eSysFaultLevel  //TBD
  {
    Normal = 0,
    LightError = 1 ,
    FatalError = 2
  };
  /*struct cellBaseReplan {
    bool replanflag;
    size_t index;
    double xg;
    double yg;
  };*/



}
}

#endif
