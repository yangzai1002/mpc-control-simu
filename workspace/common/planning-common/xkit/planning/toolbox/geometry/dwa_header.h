#ifndef COMMON_TOOLBOX_GEOMETRY_INCLUDE_DWA_HEADER_H_
#define COMMON_TOOLBOX_GEOMETRY_INCLUDE_DWA_HEADER_H_

// c++ lib
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


#include "ara/adsfi/impl_type_msgpoint3dvec.h"


// other lib
#include <bitset>
#include <deque>

#include "site.h"

/**
 * @brief The multeset obstacles tree
 */
class PtsTree {
public:
  PtsTree() {}
  PtsTree(ara::adsfi::MsgPoint3D ref) : elem(ref) {}
  ~PtsTree() {}
  ara::adsfi::MsgPoint3D elem;
  struct CompareLeafX {
    bool operator()(const ara::adsfi::MsgPoint3D lhs, const ara::adsfi::MsgPoint3D rhs) const { return lhs.x < rhs.x; }
  };
  struct CompareLeafY {
    bool operator()(const ara::adsfi::MsgPoint3D lhs, const ara::adsfi::MsgPoint3D rhs) const { return lhs.y < rhs.y; }
  };
  template<typename T>
  using default_alloc = std::allocator<T>;

  typedef std::multiset<ara::adsfi::MsgPoint3D, CompareLeafX, default_alloc<ara::adsfi::MsgPoint3D>> xtree;
  typedef std::multiset<ara::adsfi::MsgPoint3D, CompareLeafY, default_alloc<ara::adsfi::MsgPoint3D>> ytree;
};

using SortedTree =
    std::map<float, PtsTree::ytree>;



#endif
