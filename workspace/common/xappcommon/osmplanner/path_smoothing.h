#ifndef _PATH_SMOOTHING_
#define _PATH_SMOOTHING_

#include <list>
#include <memory>
#include <vector>

#include "geotool.h"
#include "xkit/planning/toolbox/geometry/bezier.h"
#include "xkit/planning/toolbox/geometry/site.h"

namespace mapplan {
    using namespace geometry;

    using SiteVec = std::vector<Site>;
    using SiteList = std::list<Site>;

    class PathProcessing {
    public:
        void SupplementPath(SiteList& path);

        bool BezierSmooth(SiteList& path) {
            Init();
            if (path.empty())
                return false; //斟酌一下
            if (HeadingCheck(path))
                return false; //斟酌一下
            auto sharp = path_sharp_iters_.begin();
            while (sharp != path_sharp_iters_.end()) {
                auto iter_start = *sharp;
                auto iter_end = iter_start;
                // auto iter_end = path_sharp_iters_.front();
                //找到异常点前一定距离的点
                double dist_to_sharp = 0.0;
                for (; std::prev(iter_start) != path.begin() && iter_start != path.begin(); --iter_start) {
                    auto iter_start_prev = std::prev(iter_start);
                    dist_to_sharp += std::hypot(iter_start_prev->x - iter_start->x, iter_start_prev->y - iter_start->y);
                    if (dist_to_sharp >= dist_to_sharp_threshold_)
                        break;
                }
                //找到异常点后一定距离的点
                dist_to_sharp = 0.0;
                for (; std::next(iter_end) != path.end() && iter_end != path.end(); ++iter_end) {
                    auto iter_end_next = std::next(iter_end);
                    dist_to_sharp += std::hypot(iter_end_next->x - iter_end->x, iter_end_next->y - iter_end->y);
                    if (dist_to_sharp >= dist_to_sharp_threshold_)
                        break;
                }
                //生成贝塞尔曲线并转换为SiteList
                auto     bezier_ptr = std::make_shared<Bezier>();
                SiteVec  partial_path_smoothed_vec;
                SiteList partial_path_smoothed_list;
                if (-1 == bezier_ptr->GenerateBezierCurve(*iter_start, *iter_end, partial_path_smoothed_vec)) {
                    std::cout << "GenerateBezierCurve failed" << std::endl;
                    return false; //斟酌一下
                }
                auto iter = partial_path_smoothed_vec.begin();
                while (std::next(iter) != partial_path_smoothed_vec.end()) {
                    partial_path_smoothed_list.emplace_back(*iter);
                    ++iter;
                }
                ++sharp;

                //替换拼接
                path.erase(std::next(iter_start), std::prev(iter_end));
                path.splice(std::next(iter_start), partial_path_smoothed_list, next(partial_path_smoothed_list.begin()),
                            partial_path_smoothed_list.end());
            }
            //插值
            auto start_iter = path.begin();
            auto end_iter = std::next(start_iter);
            while (end_iter != path.end()) {
                LinearInterpolation(start_iter, end_iter, 0.1, path);
                start_iter = end_iter;
                end_iter = std::next(start_iter);
            }
            //补全信息
            // SupplementPath(path);
            // std::cout << "after supplementpath: " <<  std::endl;
            // for(const auto &p: path){
            //     std::cout << p.x << "\t" << p.y << "\t" << p.angle << "\t" << p.xg <<
            //     "\t" << p.yg << "\t" << p.globalangle << "\t" << p.length <<
            //     std::endl;
            // }

            return true;
        }
        bool HeadingCheck(SiteList& path) {
            Init();

            if (path.empty()) {
                return false; //斟酌一下
            }
            auto iter_path = path.begin();
            auto last_sharp_iter = iter_path;
            for (; std::next(iter_path) != path.end() && iter_path != path.end(); ++iter_path) {
                auto   iter_path_next = std::next(iter_path);
                double interval = std::hypot(last_sharp_iter->x - iter_path->x, last_sharp_iter->y - iter_path->y);
                if (fabs(Normalize_Deg(iter_path_next->angle - iter_path->angle)) > path_dheading_threshold_ &&
                    interval > dist_to_sharp_threshold_ + 0.2) {
                    path_sharp_iters_.emplace_back(iter_path);
                    last_sharp_iter = iter_path;
                }
            }

            //*******
            //去除距离路径前段太近的异常点
            if (!path_sharp_iters_.empty()) {
                double dist = 0.0;
                auto   first_sharp_iter = path_sharp_iters_.front();
                for (; std::prev(first_sharp_iter) != path.begin() && first_sharp_iter != path.begin();
                     --first_sharp_iter) {
                    auto iter_first_sharp_prev = std::prev(first_sharp_iter);
                    dist += std::hypot(iter_first_sharp_prev->x - first_sharp_iter->x,
                                       iter_first_sharp_prev->y - first_sharp_iter->y);
                    if (dist > dist_to_sharp_threshold_)
                        break;
                }
                if (dist < dist_to_sharp_threshold_) {
                    path_sharp_iters_.erase(path_sharp_iters_.begin());
                }
            }
            //去除距离路径后端太近的异常点
            if (!path_sharp_iters_.empty()) {
                double dist = 0.0;
                auto   last_sharp_iter = path_sharp_iters_.back();
                for (; std::next(last_sharp_iter) != path.end() && last_sharp_iter != path.end(); ++last_sharp_iter) {
                    auto iter_last_sharp_next = std::next(last_sharp_iter);
                    dist += std::hypot(iter_last_sharp_next->x - last_sharp_iter->x,
                                       iter_last_sharp_next->y - last_sharp_iter->y);
                    if (dist > dist_to_sharp_threshold_)
                        break;
                }
                if (dist < dist_to_sharp_threshold_) {
                    path_sharp_iters_.pop_back();
                }
            }
            //*******
            if (path_sharp_iters_.empty())
                return true;
            else
                return false;
        }

        bool LinearInterpolation(const SiteList::iterator& start_iter, SiteList::iterator& end_iter,
                                 const double resolution, SiteList& result_path) {
            if (resolution < 0.1 || resolution > 5)
                return false;
            double length = std::hypot(start_iter->x - end_iter->x, start_iter->y - end_iter->y);
            if (length < 1.6 * resolution) {
                return true;
            }
            int count = static_cast<int>((length + resolution / 2) / resolution);
            if (count < 2)
                return false;
            double   detal_x = (end_iter->x - start_iter->x) / static_cast<double>(count);
            double   detal_y = (end_iter->y - start_iter->y) / static_cast<double>(count);
            double   detal_angle = Normalize_Deg(end_iter->angle - start_iter->angle) / static_cast<double>(count);
            SiteList insert_path;
            insert_path.clear();
            Site position;
            auto iter_current = end_iter;
            for (int index = 1; index < count; index++) {
                position.x = start_iter->x + detal_x * index;
                position.y = start_iter->y + detal_y * index;
                position.angle = Normalize_Deg(start_iter->angle + detal_angle * index);
                insert_path.push_back(position);
            }
            result_path.insert(end_iter, insert_path.begin(), insert_path.end());
            return true;
        }

    private:
        void Init() {
            // DataPool* DP = DataPool::Instance();
            // path_dheading_threshold_ =
            // DP->GetMainDataRef().config_info.behavior_config.path_dheading_threshold;
            // dist_to_sharp_threshold_ =
            // DP->GetMainDataRef().config_info.behavior_config.dist_to_sharp_threshold;
            // bezier_safe_dist_ =
            // DP->GetMainDataRef().config_info.behavior_config.bezier_safe_dist;
        }

        double Normalize_Deg(double angle) {
            double a = std::fmod(angle + 180.0, 2.0 * 180.0);
            if (a < 0.0) {
                a += (2.0 * 180.0);
            }
            return (a - 180.0);
        }

    private:
        double                          path_dheading_threshold_ = 30;
        double                          dist_to_sharp_threshold_ = 5.0;
        double                          bezier_safe_dist_ = 10.0;
        std::vector<SiteList::iterator> path_sharp_iters_;
    };

} // namespace mapplan
#endif
