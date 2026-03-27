/******************************************************************************
* Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
*
* NodeName: knn
* FileName: KDTree.h
*
* Description: an adaptation of the KD-tree implementation in rosetta code
*               https://rosettacode.org/wiki/K-d_tree
*              It is a reimplementation of the C code using C++.
*              It also includes a few more queries than the original
*
* History:
* Feng younan          2018/11/29          v1.0.0        Build this moldl.
******************************************************************************/
#ifndef COMMON_TOOLBOX_MATH_INCLUDE_KNN_KDTREE_H_ 
#define COMMON_TOOLBOX_MATH_INCLUDE_KNN_KDTREE_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include "xkit/planning/toolbox/geometry/geoheader.h"

namespace toolbox {
namespace math    {
namespace kdtree  {

using geometry::Site;
using geometry::SiteVec;

class KDNode;
class KDTree;

using point_t       = std::vector<double>;
using pointVec      = std::vector<point_t>;
using indexArr      = std::vector<size_t>;
using pointIndex    = typename std::pair<point_t, size_t>;
using KDNodePtr     = std::shared_ptr<KDNode>;
using pointIndexArr = typename std::vector<pointIndex>;

// Need for sorting
class comparer {
 public:
  explicit comparer(size_t dim_) : dim(dim_) {}

 public:
  bool compare_dim(const pointIndex &a, const pointIndex &b) { return (a.first.at(dim) < b.first.at(dim)); }

 public:
  size_t dim;
};

/*
 * @brief  basic element of KD tree
 * @param  none
 * @return none
 */
class KDNode {
 public:
  KDNode()  = default;
  KDNode(const point_t &pt,       //
         const size_t &idx,       //
         const KDNodePtr &left_,  //
         const KDNodePtr &right_  //
         ) : index(idx), x(pt), left(left_), right(right_) {}

  KDNode(const pointIndex &pi,    //
         const KDNodePtr &left_,  //
         const KDNodePtr &right_  //
         ) : index(pi.second), x(pi.first), left(left_), right(right_) {}

  ~KDNode() = default;

  // getter
  double coord(const size_t &idx)  { return x.at(idx); }

  // conversions
  explicit operator bool()         { return (!x.empty()); }
  explicit operator point_t()      { return x; }
  explicit operator size_t()       { return index; }
  explicit operator pointIndex()   { return pointIndex(x, index); }

 public:
  size_t    index;
  point_t   x;
  KDNodePtr left;
  KDNodePtr right;
};



class KDTree {
 public:
  KDTree()  = default;
  ~KDTree() = default;
  explicit KDTree(pointVec point_array);

 public:
  point_t       nearest_point        (const point_t &pt);
  size_t        nearest_index        (const point_t &pt);
  pointIndex    nearest_pointIndex   (const point_t &pt);
  pointIndexArr neighborhood         (const point_t &pt, const double  &rad);
  pointVec      neighborhood_points  (const point_t &pt, const double  &rad);
  indexArr      neighborhood_indices (const point_t &pt, const double  &rad);

  void          insert_point         (KDNodePtr &root, point_t &to_insert, size_t level=0);

 private:
  pointIndexArr neighborhood_ ( //
    const KDNodePtr &branch,    //
    const point_t   &pt,        //
    const double    &rad,       //
    const size_t    &level      //
  );

  KDNodePtr nearest_ (          //
    const KDNodePtr &branch,    //
    const point_t   &pt,        //
    const size_t    &level,     //
    const KDNodePtr &best,      //
    const double    &best_dist  //
  );

  KDNodePtr nearest_ (          //
    const point_t &pt           //
  );

  KDNodePtr make_tree (
    const pointIndexArr::iterator &begin,  //
    const pointIndexArr::iterator &end,    //
    const size_t                  &length, //
    const size_t                  &level   //
  );

 public:
  KDNodePtr root;
  KDNodePtr leaf;
  
  size_t    p_num ;
};


class KNN {
 public:
  explicit KNN(const SiteVec &list);
  ~KNN() = default;

 public:
  void NearestSite (Site    &result, const Site &origin)                      const;
  void NearestSite (SiteVec &result, const Site &origin, const double radius) const;
  void InsertSite  (const Site &to_insert);

 private:
  std::shared_ptr<KDTree> kd_tree_;
};



} // namespace kdtree
} // namespace math 
} // namespace toolbox
#include "kdtree.hpp"

#endif // __TOOLBOX_KNN_KDTREE_H__

