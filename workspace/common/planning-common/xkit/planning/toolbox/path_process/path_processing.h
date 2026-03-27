#ifndef _TRACK_PATH_PROCESSING_
#define _TRACK_PATH_PROCESSING_

#include <list>
#include <vector>
#include "xkit/planning/toolbox/geometry/site.h"
#include "xkit/planning/toolbox/geometry/bezier.h"
#include "xkit/planning/datapool/data_pool.h"

using namespace geometry;

namespace trackalgorithm {

using SiteVec = std::vector<geometry::Site>;
using SiteList = std::list<geometry::Site>;

class PathProcessing {
public:
    void SupplementPath(SiteList& path);
    bool BezierSmooth(SiteList& path);
    bool HeadingCheck(SiteList& path);
    bool LinearInterpolation(const SiteList::iterator& start_iter, SiteList::iterator& end_iter,
                             const double resolution, SiteList& result_path);
    bool LinearInterpolation(SiteList& result_path, const double& resolution = 0.1);
    void ThinningPath(SiteList& path, size_t num = 2);
    SiteList ExtendPath(const SiteList& path, const double& extensionDistance,
                        const int& direction = 1, const double& angle = 0,
                        const double& resolution = 0.1);
    std::pair<SiteList::iterator, double> FindNearestPoint(const double& x, const double& y,
                                                           SiteList& path);
    double NormalizeAngle(const double& x);

private:
    void Init();

private:
    double path_dheading_threshold_ = 10.0;
    double dist_to_sharp_threshold_ = 5.0;
    double bezier_safe_dist_ = 10.0;
    std::vector<SiteList::iterator> path_sharp_iters_;
};
}
#include "path_processing.hpp"

#endif
