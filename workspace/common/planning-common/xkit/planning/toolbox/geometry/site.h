/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: sitebase.h
 *
 * Description: declare 2-D points with (x, y), and some common methods
 *
 * History:
 * Feng younan          2018/08/20    v1.0.0        Build this model.
 ******************************************************************************/
#ifndef COMMON_TOOLBOX_GEOMETRY_INCLUDE_SITE_BASE_H__
#define COMMON_TOOLBOX_GEOMETRY_INCLUDE_SITE_BASE_H__

#include <cmath>
#include <functional>
#include <limits>
#include <string>

namespace geometry {

typedef struct Restricts {
  float restrict_left;
  float restrict_right;
  Restricts() {
    restrict_left = std::numeric_limits<float>::max();
    restrict_right = std::numeric_limits<float>::max();
  }
  void operator=(const Restricts &r) {
    restrict_left = r.restrict_left;
    restrict_right = r.restrict_right;
  }
} Restricts;

struct Site {
  Site() {
    x = 0;
    y = 0;
    angle = 0;
    xg = 0;
    yg = 0;
    xg_dr = 0;
    yg_dr = 0;
    globalangle = 0;
    globalangle_dr = 0;
    s = 0;
    l = 0;
    vx = 0;
    vy = 0;
    vxg = 0;
    vyg = 0;
    velocity = 0;
    index = -999;
    origin_index = -999;
    a = 0;
    t = 0;
    length = 0.0;
    width = 0.0;
    curvature = 999.0;
    speed_limit = -88.0;
    reverse = false; //false : front true: back
    type = "W";
    property = 0; // 1: global_path 2: clean_path 3: curb_path 4:connection_path
    offset_property = 0;// @pqg add 0/2
    boundary_id = "";
    pathpoint_restrict.restrict_left = std::numeric_limits<float>::max();
    pathpoint_restrict.restrict_right = std::numeric_limits<float>::max();
    dis_fence = 1.45;
    turning_around = false;
    turning_lr = false;
    turning_max = false;
  }
  Site(const Site &site) {
    x = site.x;
    y = site.y;
    angle = site.angle;
    xg = site.xg;
    yg = site.yg;
    globalangle = site.globalangle;
    xg_dr = site.xg_dr;
    yg_dr = site.yg_dr;
    globalangle_dr = site.globalangle_dr;
    s = site.s;
    l = site.l;
    vx = site.vx;
    vy = site.vy;
    vxg = site.vxg;
    vyg = site.vyg;
    velocity = site.velocity;
    index = site.index;
    origin_index = site.origin_index;
    length = site.length;
    width = site.width;
    a = site.a;
    t = site.t;
    curvature = site.curvature;
    speed_limit = site.speed_limit;
    reverse = site.reverse;
    type = site.type;
    property = site.property;
    offset_property = site.offset_property;
    boundary_id = site.boundary_id;
    pathpoint_restrict = site.pathpoint_restrict;
    dis_fence = site.dis_fence;
    turning_around = site.turning_around;
    turning_lr = site.turning_lr;
    turning_max = site.turning_max;
  }
  Site(double xx, double yy) {
    x = xx;
    y = yy;
    angle = 0;
    xg = 0;
    yg = 0;
    globalangle = 0;
    xg_dr = 0;
    yg_dr = 0;
    globalangle_dr = 0;
    s = 0.0;
    l = 0.0;
    vx = 0;
    vy = 0;
    vxg = 0;
    vyg = 0;
    velocity = 0;
    index = -999;
    origin_index = -999;
    length = 0.0;
    width = 0.0;
    a = 0;
    t = 0;
    curvature = 999.0;
    speed_limit = -88.0;
    reverse = false;
    type = "W";
    property = 0;
    offset_property = 0;
    boundary_id = "";
    pathpoint_restrict.restrict_left = std::numeric_limits<float>::max();
    pathpoint_restrict.restrict_right = std::numeric_limits<float>::max();
    dis_fence = 1.45;
    turning_around = false;
    turning_lr = false;
    turning_max = false;
  }
  Site(double xx, double yy, double aa) {
    x = xx;
    y = yy;
    angle = aa;
    xg = 0;
    yg = 0;
    globalangle = 0;
    xg_dr = 0;
    yg_dr = 0;
    globalangle_dr = 0;
    s = 0.0;
    l = 0.0;
    vx = 0;
    vy = 0;
    vxg = 0;
    vyg = 0;
    velocity = 0;
    index = -999;
    origin_index = -999;
    length = 0.0;
    width = 0.0;
    a = 0;
    t = 0;
    curvature = 999.0;
    speed_limit = -88.0;
    reverse = false;
    type = "W";
    property = 0;
    offset_property = 0;
    boundary_id = "";
    pathpoint_restrict.restrict_left = std::numeric_limits<float>::max();
    pathpoint_restrict.restrict_right = std::numeric_limits<float>::max();
    dis_fence = 1.45;
    turning_around = false;
    turning_lr = false;
    turning_max = false;
  }
  ~Site() = default;

  double x;
  double y;
  double angle;
  double xg;
  double yg;
  double globalangle;
  double xg_dr;
  double yg_dr;
  double globalangle_dr;
  double s;
  double l;
  double vx;
  double vy;
  double vxg;
  double vyg;
  double velocity;
  int index;
  int origin_index;
  double a;
  double t;
  double curvature;
  double speed_limit;
  double length;
  double width;
  bool reverse;
  std::string type;
  int property;
  int offset_property;
  std::string boundary_id;
  Restricts pathpoint_restrict;
  double dis_fence;
  bool turning_around;
  bool turning_lr;
  bool turning_max;

  double DistanceTo(const Site &s) const{
    return hypot(s.xg - xg, s.yg - yg);
  }

  void operator=(const Site &site) {
    x = site.x;
    y = site.y;
    angle = site.angle;
    xg = site.xg;
    yg = site.yg;
    globalangle = site.globalangle;
    xg_dr = site.xg_dr;
    yg_dr = site.yg_dr;
    globalangle_dr = site.globalangle_dr;
    s = site.s;
    l = site.l;
    velocity = site.velocity;
    vx = site.vx;
    vy = site.vy;
    vxg = site.vxg;
    vyg = site.vyg;
    index = site.index;
    origin_index = site.origin_index;
    a = site.a;
    t = site.t;
    length = site.length;
    width = site.width;
    curvature = site.curvature;
    speed_limit = site.speed_limit;
    reverse = site.reverse;
    type = site.type;
    property = site.property;
    offset_property = site.offset_property;
    boundary_id = site.boundary_id;
    pathpoint_restrict = site.pathpoint_restrict;
    dis_fence = site.dis_fence;
    turning_around = site.turning_around;
    turning_lr = site.turning_lr;
    turning_max = site.turning_max;
  }
  Site &operator+=(const Site &s) {
    x += s.x;
    y += s.y;
    xg += s.xg;
    yg += s.yg;
    return *this;
  }
  Site &operator-=(const Site &s) {
    x -= s.x;
    y -= s.y;
    xg -= s.xg;
    yg -= s.yg;
    return *this;
  }
  bool operator==(const Site &s) const {
    double precision = std::hypot((x - s.x), (y - s.y));
    return (precision < 1e-5) && (std::fabs(angle - s.angle) < 1e-5);
  }
  Site operator+(const Site &s) const { return Site(x + s.x, y + s.y); }
  Site operator-(const Site &s) const { return Site(x - s.x, y - s.y); }
  Site operator*(const double d) const { return Site(x * d, y * d); }
  Site operator/(const double d) const { return Site(x / d, y / d); }

  void set_g(double xx, double yy) {
    xg = xx;
    yg = yy;
  }

  void set_g(double xx, double yy, double aa) {
    xg = xx;
    yg = yy;
    angle = aa;
  }

  void set_global(double xx, double yy, double aa) {
    xg = xx;
    yg = yy;
    globalangle = aa;
  }

  void set_property(int temp_property) {
    property = temp_property;
  }

  void Reset() {
    x = 0;
    y = 0;
    angle = 0;
    xg = 0;
    yg = 0;
    globalangle = 0;
    xg_dr = 0;
    yg_dr = 0;
    globalangle_dr = 0;
    velocity = 0;
    index = -999;
    origin_index = -999;
    a = 0;
    t = 0;
    length = 0.0;
    width = 0.0;
    curvature = 999.0;
    speed_limit = -88.0;
    reverse = false; //false : front true: back
    type = "W";
    property = 0; // 1: global_path 2: clean_path 3: curb_path 4:connection_path
    offset_property = 0;
    boundary_id = "";
    pathpoint_restrict.restrict_left = std::numeric_limits<float>::max();
    pathpoint_restrict.restrict_right = std::numeric_limits<float>::max();
    dis_fence = 1.45;
    turning_around = false;
    turning_lr = false;
    turning_max = false;
  }

  double inerangle() const { return std::atan2(y, x) * 180 / M_PI; }
  double mold() const { return std::hypot(x, y); }
  Site direction() const { return Site(x / this->mold(), y / this->mold()); }

  struct compare {
    bool operator()(const Site &s1, const Site &s2) const {
      if (std::fabs(s1.x - s2.x) < 1e-5)
        return s1.y < s2.y;
      else
        return s1.x < s2.x;
    }
  };

  struct hash_equal {
    bool operator()(const Site &ls, const Site &rs) const {
      double precision = std::hypot((ls.x - rs.x), (ls.y - rs.y));
      return (precision < 1e-5);
    }
  };

  struct hash_key {
    std::size_t operator()(const Site &s) const {
      std::hash<double> hash_fun;
      return size_t(hash_fun(s.x + s.y));
    }
  };
};

  struct s_equal
  {
    s_equal(double n) : s(n) { } 
    bool operator()(Site& p) 
    { 
      return (s == p.s); 
    } 
    int s;
  };

struct StructTtajectoryPoint : public Site {
  
};

}  // namespace geometry

#endif  // __COMMON_GEOMETRY_SITE_BASE_H__
