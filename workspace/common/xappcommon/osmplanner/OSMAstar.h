//
// Created by syn on 22-9-6.
//

#ifndef OSM_OSMASTAR_H
#define OSM_OSMASTAR_H

#include <chrono>
#include <fstream>
#include <vector>

#include "AstarBase.h"
#include "log/log.h"
#include "nanoflann.h"
namespace mapplan {
    struct DPNode {
        using Ptr = DPNode*;
        OSMId              id_;
        OSMId              node_id_;
        DPNode::Ptr        prev_node_;
        double             min_cost_;
        std::vector<OSMId> min_cost_path;
        OSMId              failed_edge_id_ = -1;

        DPNode() {
            id_ = 0;
            node_id_ = 0;
            min_cost_ = INFINITY;
            prev_node_ = nullptr;
        }

        DPNode(const OSMId id, DPNode::Ptr parent, double cost) : prev_node_(parent), min_cost_(cost), id_(id) {
            min_cost_ = INFINITY;
            node_id_ = 0;
        };

        DPNode(const OSMId id, DPNode::Ptr parent) : prev_node_(parent), id_(id) {
            min_cost_ = INFINITY;
            node_id_ = 0;
        };

        void UpdateCost(const DPNode::Ptr node_prev, const double& cost, const std::vector<OSMId>& part_path) {
            if (cost <= min_cost_) {
                //            std::cout << "update" <<'\n';
                min_cost_ = cost;
                prev_node_ = node_prev;
                min_cost_path.clear();
                std::copy(part_path.begin(), part_path.end(), std::back_inserter(min_cost_path));
            }
        }

        void UpdateFailedEdge(const OSMId failed_edge_id) {
            failed_edge_id_ = failed_edge_id;
        }
    };

    struct DPNodeMessage {
        DPNodeMessage(std::list<DPNode>& prev_nodes, DPNode::Ptr cur_node)
            : prev_nodes_(prev_nodes), cur_node_(cur_node) {};

        std::list<DPNode>& prev_nodes_;
        DPNode::Ptr        cur_node_ = nullptr;
    };

    class OSMAstar {
    public:
        explicit OSMAstar() {
            _astar_base = std::make_shared<AstarBase>();
            _source_id = -9999;
            _target_id = -9999;
            _start_id = -9999;
            _end_id = -9999;

            AINFO << " start build kd tree";
            //========================== build kd tree =========================//
            cloud.pts.clear();
            for (const auto& e : _rng.getEdges()) {
                for (const auto& n : e.second.EdgePoints) {
                    cloud.pts.emplace_back(_db.nodeFromID(n)->getLat(), _db.nodeFromID(n)->getLon(), n, e.first);
                }
            }
            index = new kdTree(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        }

        ~OSMAstar() {
            if (index) {
                delete index;
            }
        }

        int SearchBasedPoint(const std::vector<latlon>& mission_points, const std::vector<latlon>& block_points,
                             std::vector<latlon>& result_path) {
            int rtvalue = -1;

            //================================process mission
            // points=============================//
            std::vector<OSMId> map_points;
            OSMId              node_id, edge_id;

            if (mission_points.size() < 2) {
                AERROR << "at least need two poinst";
                return rtvalue;
            }
            auto   iter = mission_points.begin();
            size_t index = 0;
            for (; iter != mission_points.end(); ++iter) {
                bool is_vertex = false;

                int rtv_flag = FindNearestNode(iter, node_id, edge_id, is_vertex);
                if (rtv_flag == -1) {
                    AERROR << " find nearest node failed";
                    return rtvalue;
                }
                // ruguo dangqian missio point is too nearest to last mission point
                // continue;
                if (rtv_flag == 1) {
                    continue;
                }

                _points_ll.emplace_back(*iter);
                map_points.emplace_back(node_id);
                _source_target_vec.emplace_back(std::make_tuple(edge_id, node_id, is_vertex, iter->yaw_valid));
            }
            //================================ finish process mission
            // points==============================//

            //================================== process block points
            //=====================================//
            OSMId block_node_id, block_edge_id;
            bool  is_block_vertex = false;
            bool  is_replan_flag = false;
            AINFO << "==================== start find block======================";
            for (const auto& bp : block_points) {
                if (FindBlockEdge(bp, block_node_id, block_edge_id, is_block_vertex) == 1) {
                    AERROR << " find block node failed";
                    continue;
                }

                if (is_block_vertex) {
                    if (_rng.AddForbiddenArea(block_node_id, is_block_vertex, is_replan_flag) == -1) {
                        AINFO << " add block failed ";
                        Clear();
                        return -1;
                    };
                    continue;
                }

                if (std::fabs(block_edge_id) == std::fabs(std::get<0>(_source_target_vec.front()))) {
                    AINFO << " replan car in block edge ";
                    is_replan_flag = true;
                    if (SelectHeadingEdge(block_edge_id, std::get<1>(_source_target_vec.front()),
                                          _points_ll.front().yaw) == -1) {
                        AERROR << "select edge based car heading failed";
                        return -1;
                    }

                    // 判断禁行点在车辆当前路段前方还是后方
                    if (!IsPointBehindAnotherPoint(block_edge_id, std::get<1>(_source_target_vec.front()),
                                                   block_node_id)) {
                        std::cout << " block point is behind car, edge id is " << -block_edge_id << '\n';
                        if (_rng.AddForbiddenArea(
                                -block_edge_id, is_block_vertex,
                                is_replan_flag)) { // 根据pm需求更改，只将车辆当前方向的路段号加入禁止通行路段
                            std::cout << " add car heading block edge failed " << '\n';
                            Clear();
                            return -1;
                        }
                    } else {
                        AINFO << " block point is front car , edge id is " << block_edge_id;
                        if (_rng.AddForbiddenArea(
                                block_edge_id, is_block_vertex,
                                is_replan_flag)) { // 根据pm需求更改，只将车辆当前方向的路段号加入禁止通行路段
                            AERROR << " add car heading block edge failed ";
                            Clear();
                            return -1;
                        }

                        if (_points_ll.front().yaw < 0) {
                            _points_ll.front().yaw = _points_ll.front().yaw + 360;
                        }

                        if (_points_ll.front().yaw > 180) {
                            _points_ll.front().yaw = _points_ll.front().yaw - 180;
                        } else if (_points_ll.front().yaw < 180) {
                            _points_ll.front().yaw = _points_ll.front().yaw + 180;
                        }
                    }
                    continue;
                }
                if (_rng.AddForbiddenArea(block_edge_id, is_block_vertex, is_replan_flag) == -1) {
                    AINFO << " add block failed ";
                    Clear();
                    return -1;
                };
            }
            //================================= finish process block points
            //===================================//

            _start_ll.lat = _db.nodeFromID(map_points.front())->getLat();
            _start_ll.lon = _db.nodeFromID(map_points.front())->getLon();
            _end_ll.lat = _db.nodeFromID(map_points.back())->getLat();
            _end_ll.lon = _db.nodeFromID(map_points.back())->getLon();
            _start_id = map_points.front();
            _end_id = map_points.back();

            //======================== get candiate edges of each mission points
            //==============================//
            std::vector<std::vector<OSMId>> candiate_edges;
            auto                            time1 = std::chrono::steady_clock::now();
            if (GetCandiateEdges(_source_target_vec, candiate_edges) == -1) {
                AERROR << " get candiate edges failed " << '\n';
                Clear();
                return rtvalue;
            }
            auto time2 = std::chrono::steady_clock::now();
            auto candiate_time = std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1).count();
            AINFO << " get cnadiate time " << candiate_time << "us";
            //===================== finish get candiate edges
            //=============================================//

            //======================== DP process get shortest path
            //======================================//
            if (SearchBasedDP(candiate_edges) == -1) {
                AERROR << " DP search failed " << '\n';
                _result_roadinfo.clear();
                Clear();
                return rtvalue;
            }
            //======================== finsih DP process get shortest path
            //===============================//

            result_path = _result_path;
            Clear();
            rtvalue = 0;
            return rtvalue;
        }

        int SearchBaseRoadId(const std::vector<OSMId>& road_ids, std::vector<latlon>& result_path) {
            int rtvalue = -1;
            result_path.clear();
            auto edges = _rng.getEdges();
            AINFO << "SearchBaseRoadId";
            for (const auto& r : road_ids) {
                if (edges.find(r) == edges.end()) {
                    AERROR << "[ERROR] : road id is not in graph" << '\n';
                    return rtvalue;
                }
                AINFO << "_rng.getEdge(r).edgepoints_ll SIZE = " << _rng.getEdge(r).edgepoints_ll.size();
                //  result_path.reserve(result_path.size() + _rng.getEdge(r).edgepoints_ll.size());
                //     std::copy(_rng.getEdge(r).edgepoints_ll.begin(), _rng.getEdge(r).edgepoints_ll.end(),
                //               std::back_inserter(result_path));
                result_path = _rng.getEdge(r).edgepoints_ll;
                AINFO << "result_path SIZE = " << result_path.size();
            }
            rtvalue = 0;
            return rtvalue;
        }

        void OutputRoadInfo(std::vector<RoadInfo>& result_roadinfo) {
            if (_result_roadinfo.empty()) {
                AERROR << "road info is empty " << '\n';
                return;
            }
            result_roadinfo = std::move(_result_roadinfo);
        }

        void Reset() {
            Clear();
            cloud.pts.clear();
            if (index != nullptr) {
                delete index;
            }
            for (const auto& e : _rng.getEdges()) {
                for (const auto& n : e.second.EdgePoints) {
                    cloud.pts.emplace_back(_db.nodeFromID(n)->getLat(), _db.nodeFromID(n)->getLon(), n, e.first);
                }
            }

            index = new kdTree(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
        }

    private:
        void Clear() {
            _result_path.clear();
            _source_target_vec.clear();
            _points_ll.clear();
            _graph_nodes.clear();
            _rng.CleanForbiddenArea();
            candiate_nodes_.clear();
        }

        int SearchPath(std::vector<OSMId>& result, const OSMId& first_point, const OSMId& second_point) {
            int                 rtvalue = -1;
            std::vector<latlon> result_path_tmp;
            std::vector<OSMId>  result_roadinfo_tmp;

            // std::cout << "=================================" << '\n';
            // std::cout << "source edge id  " << _source_id << "   target : " <<
            // _rng->getEdge(_source_id).target << '\n'; std::cout << "target edge id  "
            // << _target_id<< "   target : " << _rng->getEdge(_target_id).source <<
            // '\n'; std::cout << "==================================" << '\n';

            if (_source_id == _target_id && !IsPointBehindAnotherPoint(_source_id, first_point, second_point)) {
                // std::cout << "[INFO] : start search same edge " <<'\n';
                if (_astar_base->SearchBasedVertex(_rng.getEdge(_source_id).target, _rng.getEdge(_target_id).source,
                                                   _source_id, result) == -1) {
                    // std::cout << "[ERROR] : same Astar seach path failed !!!" <<
                    // '\n';
                    return rtvalue;
                }
                // std::cout << "size  ssdsdsads " <<result.size() <<'\n';

                // std::cout <<"[INFO] : same edge astar serach finish!" << '\n';
                rtvalue = 0;
                return rtvalue;
            }

            // std::cout <<"[INFO] : astar serach start!" << '\n';
            if (_astar_base->SearchBase(_source_id, _target_id, result) == -1) {
                // AERROR << "[ERROR] : Astar seach path failed !!!" << '\n';
                return rtvalue;
            }
            // AINFO <<"[INFO] : astar serach finish!" << '\n';

            rtvalue = 0;
            return rtvalue;
        }

        int FindNearestNode(std::vector<latlon>::const_iterator& ll, OSMId& node_id, OSMId& edge_id, bool& is_vertex) {
            int rtvalue = -1;
            // kd-tree optimize
            const size_t                    numResults = 1;
            size_t                          resIndex;
            double                          outDistSqr;
            double                          queryPt[2] = {ll->lat, ll->lon};
            nanoflann::KNNResultSet<double> resultSet(numResults);
            resultSet.init(&resIndex, &outDistSqr);
            index->findNeighbors(resultSet, queryPt, nanoflann::SearchParams());

            node_id = cloud.pts.at(resIndex).node_id_;
            edge_id = cloud.pts.at(resIndex).edge_id_;

            AINFO << " ll " << ll->lat << " " << ll->lon << " edgeid " << edge_id << "  node_id  " << node_id << '\n';

            const double out_dist = Coord::ComputeDist(ll->lat, ll->lon, _db.nodeFromID(node_id)->getLat(),
                                                       _db.nodeFromID(node_id)->getLon());
            // 输入了错误的任务点，距离graph的最近距离小于30m（可调）;
            //  TODO : 100  lzr
            if (out_dist > 25000) {
                rtvalue = -1;
                AERROR << "[ERROR] : the mission point is too far to graph" << '\n';
                return rtvalue;
            }

            // 如果当前匹配点-任务点距离vertex在一定范围内，认为是匹配点为该顶点
            if (ll->type == ePointType::MISSION) {
                const auto& ll_to_node_dist = Coord::ComputeDist(_db.nodeFromID(node_id)->getLat(),
                                                                 _db.nodeFromID(node_id)->getLon(), ll->lat, ll->lon);

                std::multimap<double, OSMId> candiate_vertexs;
                std::multimap<double, OSMId> candiate_dist_to_vertexs;
                for (const auto& v : _rng.getVeterxs()) {
                    double ll_to_vertex_dist = Coord::ComputeDist(ll->lat, ll->lon, _db.nodeFromID(v.first)->getLat(),
                                                                  _db.nodeFromID(v.first)->getLon());

                    candiate_dist_to_vertexs.insert(std::make_pair(ll_to_vertex_dist, v.first));

                    if (ll_to_vertex_dist < 15) {
                        if (std::get<2>(_source_target_vec.back())) {
                            if (std::get<1>(_source_target_vec.back()) == v.first) {
                                continue;
                            }
                        }
                        double cost;
                        ProcessJunctionPoint(*std::prev(ll), *std::next(ll), v.first, cost);
                        // normalize 0~1;
                        //                ll_to_vertex_dist = ll_to_vertex_dist/cost;
                        double cost_tmp = (cost * 0.1 + ll_to_vertex_dist * 0.9);
                        std::cout << "from  " << v.first << " cost " << cost_tmp << '\n';
                        candiate_vertexs.insert(std::make_pair(cost_tmp, v.first));
                        cost = 0;
                    }
                }

                if (candiate_dist_to_vertexs.empty()) {
                    return rtvalue;
                }
                // if node is nearset to lane , directly think it is on lane

                if (ll_to_node_dist <= 1 && candiate_dist_to_vertexs.begin()->first > 1) {
                    std::cout << "is nearest lane " << '\n';
                    return 0;
                }
                // nearest to lane  and nearest to vertex
                if (ll_to_node_dist <= 2 && candiate_dist_to_vertexs.begin()->first <= 3) {
                    std::cout << " is nearset vertex" << '\n';
                    std::cout << "candiate vertex size " << candiate_vertexs.size() << '\n';
                    std::cout << "is vertex " << '\n';

                    is_vertex = true;
                    node_id = candiate_dist_to_vertexs.begin()->second;
                    std::cout << " vettex id  " << node_id << '\n';
                }
                // nearset to lane
                else if (ll_to_node_dist <= 2 && candiate_dist_to_vertexs.begin()->first > 3) {
                    std::cout << " is on edge " << '\n';

                    const auto& last_node_id = std::get<1>(_source_target_vec.back());
                    const auto& last_edge_id = std::get<0>(_source_target_vec.back());

                    const auto& ll_to_last_ll = Coord::ComputeDist(
                        _db.nodeFromID(node_id)->getLat(), _db.nodeFromID(node_id)->getLon(),
                        _db.nodeFromID(last_node_id)->getLat(), _db.nodeFromID(last_node_id)->getLon());
                    if (ll_to_last_ll < 8 && last_edge_id == edge_id) {
                        std::cout << "current misison point is too nearest to last "
                                     "mission point "
                                  << '\n';
                        rtvalue = 1;
                        return rtvalue;
                    }
                }
                // estimate vertex
                else {
                    if (!candiate_vertexs.empty()) {
                        std::cout << " estimate vertex" << '\n';
                        std::cout << "candiate vertex size " << candiate_vertexs.size() << '\n';
                        std::cout << "is vertex " << '\n';

                        is_vertex = true;
                        node_id = candiate_vertexs.begin()->second;
                        std::cout << " vettex id  " << node_id << '\n';
                    }
                }
            }

            return 0;
        }

        int GetCandiateEdges(const std::vector<std::tuple<OSMId, OSMId, bool, bool>>& source_target_vec,
                             std::vector<std::vector<OSMId>>&                         candiate_edges) {
            int rtvalue = -1;

            if (source_target_vec.empty()) {
                AERROR << "source target vec is empty " << '\n';
                return rtvalue;
            }

            std::cout << __LINE__ << " " << __FUNCTION__ << " start get candiate edges " << '\n';
            for (size_t i = 0; i < source_target_vec.size(); ++i) {
                // source
                auto               edgeId_s = std::get<0>(source_target_vec.at(i));
                auto               source_node_id = std::get<1>(source_target_vec.at(i));
                auto               is_vertex_s = std::get<2>(source_target_vec.at(i));
                auto               yaw_valid_s = std::get<3>(source_target_vec.at(i));
                std::vector<OSMId> candiates;

                //==================== yaw valid ================================//
                if (yaw_valid_s) {
                    if (_rng.getEdge(edgeId_s).isOneWay) {
                        candiates.emplace_back(edgeId_s);
                    } else if (is_vertex_s) {
                        // vertex
                        std::multimap<double, OSMId> azi_edge_id_map;
                        for (const auto& e : _rng.getVertex(source_node_id).out_edges) {
                            // if edge is blocked
                            if (!_rng.getEdge(e).visited) {
                                continue;
                            }

                            auto edge_points_tmp = _rng.getEdge(e).EdgePoints;
                            if (edge_points_tmp.empty()) {
                                AERROR << "[ERROR] : cant get edgepoint from edge " << e;
                                return rtvalue;
                            }
                            auto iter_begin = edge_points_tmp.begin();
                            auto iter_move_forward = iter_begin;
                            if (std::distance(iter_begin, edge_points_tmp.end()) < 11)
                                iter_move_forward = std::prev(edge_points_tmp.end());
                            else
                                std::advance(iter_move_forward, 10);

                            double azi_forward = Coord::ComputeAzimuth(_db.nodeFromID(*iter_begin)->getLat(),
                                                                       _db.nodeFromID(*iter_begin)->getLon(),
                                                                       _db.nodeFromID(*iter_move_forward)->getLat(),
                                                                       _db.nodeFromID(*iter_move_forward)->getLon());

                            auto& point_yaw = _points_ll.at(i).yaw;
                            AINFO << " mission point  yaw before normal  " << point_yaw;
                            if (point_yaw < 0) {
                                point_yaw += 360;
                            }

                            AINFO << " mission point  yaw after normal  " << point_yaw;

                            double azi_minus = std::fabs(azi_forward - point_yaw);
                            QuadrantChecker(azi_minus);
                            if (!_rng.getEdge(e).is_dead_end)
                                azi_edge_id_map.insert(std::make_pair(azi_minus, e));
                        }
                        candiates.emplace_back(azi_edge_id_map.begin()->second);
                    } else {
                        // 计算航向选择起始边
                        auto edge_points = _rng.getEdge(edgeId_s).EdgePoints;
                        if (edge_points.empty()) {
                            AERROR << "[ERROR] : cant get edgetpoint from edge " << edgeId_s;
                            return rtvalue;
                        }
                        auto iter = std::find(edge_points.begin(), edge_points.end(), source_node_id);
                        if (iter == edge_points.end()) {
                            AERROR << "source node id :" << source_node_id << " is not in edge : " << edgeId_s << '\n';
                            return rtvalue;
                        }

                        // 向前移动的迭代器，向前移动10个距离，然后计算航向角
                        auto iter_move_forward = iter;
                        auto iter_move_backward = iter;
                        // 前后移动当前点的迭代器，后续计算 航向角选择起始edge
                        if (std::distance(iter, edge_points.end()) < 11)
                            iter_move_forward = std::prev(edge_points.end());
                        else
                            std::advance(iter_move_forward, 10); // todo

                        if (std::distance(edge_points.begin(), iter) < 11)
                            iter_move_backward = edge_points.begin();
                        else
                            std::advance(iter_move_backward, -10);

                        double azi_forward = Coord::ComputeAzimuth(
                            _db.nodeFromID(*iter)->getLat(), _db.nodeFromID(*iter)->getLon(),
                            _db.nodeFromID(*iter_move_forward)->getLat(), _db.nodeFromID(*iter_move_forward)->getLon());
                        double azi_backward =
                            Coord::ComputeAzimuth(_db.nodeFromID(*iter)->getLat(), _db.nodeFromID(*iter)->getLon(),
                                                  _db.nodeFromID(*iter_move_backward)->getLat(),
                                                  _db.nodeFromID(*iter_move_backward)->getLon());

                        std::cout << "azi forward : " << azi_forward << "  edge id " << edgeId_s << '\n';
                        std::cout << "azi backward : " << azi_backward << "  edge id " << -edgeId_s << '\n';

                        auto& point_yaw = _points_ll.at(i).yaw;

                        AINFO << " mission point  yaw before normal  " << point_yaw;

                        if (point_yaw < 0) {
                            point_yaw += 360;
                        }
                        AINFO << " mission point  yaw after normal  " << point_yaw;

                        double delta_forward = std::fabs(azi_forward - point_yaw);
                        double delta_backward = std::fabs(azi_backward - point_yaw);
                        QuadrantChecker(delta_forward);
                        QuadrantChecker(delta_backward);

                        OSMId heading_edge = delta_forward < delta_backward ? edgeId_s : -edgeId_s;
                        bool  is_same_and_after = false;
                        if (i == 0) { // 当前仅针对自车位置
                            if (std::fabs(edgeId_s) == std::fabs(std::get<0>(source_target_vec.at(i + 1))))
                                is_same_and_after = IsPointBehindAnotherPoint(heading_edge, source_node_id,
                                                                              std::get<1>(source_target_vec.at(i + 1)));
                        }

                        std::cout << "is dead end : [ " << _rng.getEdge(heading_edge).is_dead_end
                                  << " ] ; is same and after [ " << is_same_and_after << " ] " << '\n';
                        // 如果自车航向与当前edge航向相同并且当前edge不为断头路，则当前edge为候选边，否则则选择反向路，向自车后向搜索
                        //  and next point is in samse edge as current point and is not
                        //  reverse -----2023.2.24根据A3需求增加 syn
                        if (_rng.getEdge(heading_edge).is_dead_end && !is_same_and_after && i == 0)
                            candiates.emplace_back(-heading_edge);
                        else
                            candiates.emplace_back(heading_edge);
                    }

                    if (candiates.empty()) {
                        std::cout << " cant throuth some level" << '\n';
                        return rtvalue;
                    }

                    if (i > 0) {
                        OSMId process_candiate_edge;
                        int   rtv_flag = ProcessCandiateEdges(
                            candiate_edges.back(), candiates, std::get<1>(source_target_vec.at(i - 1)),
                            std::get<1>(source_target_vec.at(i)), process_candiate_edge);

                        if (rtv_flag == 1) {
                            candiate_edges.back().clear();
                            candiate_edges.back().emplace_back(process_candiate_edge);
                            continue;
                        } else if (rtv_flag == -1)
                            return rtvalue;
                        else if (rtv_flag == 2) {
                            continue;
                        }
                    }

                    candiate_edges.emplace_back(candiates);
                    candiate_nodes_.emplace_back(source_node_id);
                    continue;
                }

                if (is_vertex_s) {
                    auto out_edges = _rng.getVertex(source_node_id).out_edges;
                    auto in_edges = _rng.getVertex(source_node_id).in_edges;
                    if (out_edges.empty()) {
                        AERROR << "  get vertex out edges failed " << '\n';
                        return rtvalue;
                    }
                    for (const auto& e : in_edges) {
                        if (!_rng.getEdge(e).visited) {
                            continue;
                        }
                        std::cout << "candiate edges : " << e << '\n';
                        candiates.emplace_back(e);
                    }
                    for (const auto& e : out_edges) {
                        if (!_rng.getEdge(e).visited) {
                            continue;
                        }
                        std::cout << "candiate edges : " << e << '\n';
                        candiates.emplace_back(e);
                    }
                } else if (_rng.getEdge(edgeId_s).isOneWay) {
                    candiates.emplace_back(edgeId_s);
                } else {
                    candiates.emplace_back(edgeId_s);
                    candiates.emplace_back(-edgeId_s);
                }

                if (candiates.empty()) {
                    std::cout << " cant through some level " << '\n';
                    return rtvalue;
                }

                if (i > 0) {
                    OSMId process_candiate_edge;
                    int   rtv_flag =
                        ProcessCandiateEdges(candiate_edges.back(), candiates, std::get<1>(source_target_vec.at(i - 1)),
                                             std::get<1>(source_target_vec.at(i)), process_candiate_edge);
                    if (rtv_flag == 1) {
                        candiate_edges.back().clear();
                        candiate_edges.back().emplace_back(process_candiate_edge);
                        candiate_nodes_.back() = source_node_id;
                        continue;
                    } else if (rtv_flag == -1) {
                        return rtvalue;
                    } else if (rtv_flag == 2) {
                        candiate_nodes_.back() = source_node_id;
                        continue;
                    }
                }

                candiate_edges.emplace_back(candiates);
                candiate_nodes_.emplace_back(source_node_id);
            }

            //=========================print================================//
            for (const auto& s : candiate_edges) {
                for (const auto& ss : s) {
                    std::cout << ss << " ";
                }
                std::cout << '\n';
            }
            //==============================================================//
            std::cout << __LINE__ << " " << __FUNCTION__ << " finish get candiate edges " << '\n';
            rtvalue = 0;
            return rtvalue;
        }

        void CheckRoadInfo(std::vector<OSMId>& road_infos) {
            road_infos.erase(std::unique(road_infos.begin(), road_infos.end()), road_infos.end());
        }

        int SearchBasedDP(const std::vector<std::vector<OSMId>>& candiate_edges) {
            std::cout << "start dp " << '\n';
            int rtvalue = -1;

            if (candiate_edges.empty()) {
                AERROR << " candiate edge is empty" << '\n';
                return rtvalue;
            }

            _graph_nodes.emplace_back();
            _graph_nodes.back().emplace_back(candiate_edges.front().front(), nullptr, 0);
            _graph_nodes.back().back().min_cost_path.emplace_back(candiate_edges.front().front());
            _graph_nodes.back().back().node_id_ = candiate_nodes_.front();
            _graph_nodes.back().back().min_cost_ = 0;

            AINFO << " fisrt level " << candiate_edges.front().front();

            //============================ DP Process===============================//
            auto time1 = std::chrono::steady_clock::now();
            for (size_t level = 1; level < candiate_edges.size(); ++level) {
                const auto& level_edges = candiate_edges.at(level);
                auto&       prev_nodes = _graph_nodes.back();

                _graph_nodes.emplace_back();
                int faild_num = 0;

                for (size_t i = 0; i < level_edges.size(); ++i) {
                    const auto& edge = level_edges.at(i);

                    _graph_nodes.back().emplace_back(edge, nullptr);
                    _graph_nodes.back().back().node_id_ = candiate_nodes_.at(level);

                    auto msg = std::make_shared<DPNodeMessage>(prev_nodes, &_graph_nodes.back().back());
                    UpdateNode(msg);
                    if (_graph_nodes.back().back().min_cost_ == INFINITY) {
                        ++faild_num;
                    }
                }
                if (faild_num == level_edges.size()
                    //&&
                    //!_rng->getEdge(_graph_nodes.back().back().failed_edge_id_).is_dead_end
                ) {
                    AERROR << "level " << level
                           << " can not arrive, because last level edge :" << _graph_nodes.back().back().failed_edge_id_
                           << '\n';
                    return rtvalue;
                }
            }

            //================================ finish DP Process
            //========================//
            auto time2 = std::chrono::steady_clock::now();
            auto dp_time = std::chrono::duration_cast<std::chrono::milliseconds>(time2 - time1).count();

            // std::cout << "dp time is " << dp_time << "ms " << '\n';

            // std::cout << " graph  " <<'\n';
            // for (const auto& g :_graph_nodes) {

            //     for (const auto& n :g) {
            //         for (const auto ss : n.min_cost_path) {
            //             std::cout << ss << " ";
            //         }
            //         std::cout << '\n';
            //     }
            //     std::cout << '\n';
            // }

            //=============================== Trace
            // Back==================================//
            DPNode fake_head;
            for (auto& dp_node : _graph_nodes.back()) {
                fake_head.UpdateCost(&dp_node, dp_node.min_cost_, dp_node.min_cost_path);
            }

            // trace back
            std::vector<OSMId> result_road_info;
            if (candiate_edges.size() == 1) {
                result_road_info.emplace_back(_graph_nodes.front().front().id_);
            } else {
                DPNode::Ptr min_cost_node = &fake_head;
                while (min_cost_node->prev_node_) {
                    min_cost_node = min_cost_node->prev_node_;
                    std::copy(min_cost_node->min_cost_path.rbegin(), min_cost_node->min_cost_path.rend(),
                              std::back_inserter(result_road_info));
                }

                std::cout << "ssssss" << '\n';
                for (const auto& s : result_road_info) {
                    std::cout << s << "  ";
                }
                std::cout << '\n';

                CheckRoadInfo(result_road_info);

                std::reverse(result_road_info.begin(), result_road_info.end());
            }

            if (result_road_info.empty()) {
                std::cout << " result road info is empty" << '\n';
                return rtvalue;
            }
            if (result_road_info.front() != _graph_nodes.front().front().id_) {
                std::cout << " is not continues " << '\n';
                return rtvalue;
            }

            //========================== finish trace back
            //=====================================//

            auto t1 = std::chrono::steady_clock::now();

            if (result_road_info.size() == 1) {
                AERROR << " is one road ";
                RoadInfo road_info;
                road_info.edgeId = result_road_info.front();
                // road_info.direction = 1;
                // if (result_road_info.front() < 0) {
                //     road_info.edgeId = -1 * result_road_info.front();
                //     road_info.direction = 0;
                // }
                road_info.invalid = false;
                _result_roadinfo.emplace_back(road_info);
                const auto& edgepoint_ll = _rng.getEdge(result_road_info.front()).edgepoints_ll;

                auto iter_sourece = std::find_if(edgepoint_ll.begin(), edgepoint_ll.end(), [&](const latlon& ll) {
                    double precision = std::hypot((ll.lat - _start_ll.lat), (ll.lon - _start_ll.lon));
                    return (precision < 1e-5);
                });

                auto iter_target = std::find_if(edgepoint_ll.begin(), edgepoint_ll.end(), [&](const latlon& ll) {
                    double precision = std::hypot((ll.lat - _end_ll.lat), (ll.lon - _end_ll.lon));
                    return (precision < 1e-5);
                });

                if (iter_sourece == edgepoint_ll.end() || iter_target == edgepoint_ll.end()) {
                    return rtvalue;
                }
                std::copy(iter_sourece, iter_target, std::back_inserter(_result_path));

                // for (int i = _start_id; i != _end_id; ++i) {
                //     _result_path.emplace_back(edgepoint_ll.at(i));
                // }
                // 单段回环路
                if (_db.wayFromID(std::fabs(result_road_info.front()))->isClosed()) {
                    AINFO << " is closie sss";
                    if (iter_sourece > iter_target) {
                        std::copy(iter_sourece, edgepoint_ll.end(), std::back_inserter(_result_path));
                        std::copy(edgepoint_ll.begin(), iter_target, std::back_inserter(_result_path));
                    } else if (iter_sourece == iter_target) {
                        std::copy(edgepoint_ll.begin(), edgepoint_ll.end(), std::back_inserter(_result_path));
                    }
                }

                rtvalue = 0;
                return rtvalue;
            }

            for (size_t i = 0; i < result_road_info.size(); ++i) {
                RoadInfo road_info;
                road_info.edgeId = result_road_info.at(i);
                //road_info.direction = 1;
                // if (road_info.edgeId < 0) {
                //     road_info.edgeId = -1 * road_info.edgeId;
                //     road_info.direction = 0;
                // }
                road_info.invalid = false;
                _result_roadinfo.emplace_back(road_info);

                auto edgepoint_ll = _rng.getEdge(road_info.edgeId).edgepoints_ll;

                if (i == 0) {
                    auto iter = std::find_if(edgepoint_ll.begin(), edgepoint_ll.end(), [&](const latlon& ll) {
                        double precision = std::hypot((ll.lat - _start_ll.lat), (ll.lon - _start_ll.lon));
                        return (precision < 1e-5);
                    });
                    if (iter == edgepoint_ll.end()) {
                        AERROR << " _start ll : " << _start_ll.lat << " " << _start_ll.lon << " is not in edge "
                               << result_road_info.at(i) << '\n';
                        return rtvalue;
                    }

                    std::copy(iter, edgepoint_ll.end(), std::back_inserter(_result_path));
                    continue;
                }

                if (i == result_road_info.size() - 1) {
                    auto iter = std::find_if(edgepoint_ll.begin(), edgepoint_ll.end(), [&](const latlon& ll) {
                        double precision = std::hypot((ll.lat - _end_ll.lat), (ll.lon - _end_ll.lon));
                        return (precision < 1e-5);
                    });

                    if (iter == edgepoint_ll.end()) {
                        AERROR << " _end ll : " << _end_ll.lat << " " << _end_ll.lon << " is not in edge "
                               << result_road_info.at(i) << '\n';
                        return rtvalue;
                    }

                    if (_rng.getVertex(_rng.getEdge(result_road_info.at(i)).source).nodeID !=
                        _rng.getVertex(_rng.getEdge(result_road_info.at(i - 1)).target).nodeID) {
                        AERROR << " global path is not continus "
                               << "\n";
                        return rtvalue;
                    }

                    std::copy(edgepoint_ll.begin(), iter, std::back_inserter(_result_path));
                    continue;
                }

                //----------------------------- deadend edge
                //---------------------------------------//
                if (std::fabs(result_road_info.at(i)) == std::fabs(result_road_info.at(i + 1))) {
                    std::cout << "[MERGE_PATH] : is in dead edge" << '\n';
                    auto iter_of_node =
                        std::find_if(_source_target_vec.begin(), _source_target_vec.end(),
                                     [&](const std::tuple<OSMId, OSMId, bool, bool>& tpl) -> bool {
                                         return std::fabs(std::get<0>(tpl)) == std::fabs(result_road_info.at(i));
                                     });
                    if (iter_of_node == _source_target_vec.end()) {
                        AERROR << __FUNCTION__ << " [deadend find iter failed]";
                        return -1;
                    }
                    auto iter_of_path = std::find_if(edgepoint_ll.begin(), edgepoint_ll.end(), [&](const latlon& ll) {
                        double precision = std::hypot((ll.lat - _db.nodeFromID(std::get<1>(*iter_of_node))->getLat()),
                                                      (ll.lon - _db.nodeFromID(std::get<1>(*iter_of_node))->getLon()));
                        return (precision < 1e-5);
                    });
                    if (iter_of_path == edgepoint_ll.end()) {
                        std::cout << " _end ll : " << _end_ll.lat << " " << _end_ll.lon << " is not in edge "
                                  << result_road_info.at(i) << '\n';
                        return rtvalue;
                    }
                    std::copy(edgepoint_ll.begin(), iter_of_path, std::back_inserter(_result_path));

                    _result_path.back().is_reverse = true; // set is_reverse flag is true, easy for processing road
                                                           // point
                    std::vector<latlon> temp;
                    std::copy(edgepoint_ll.begin(), iter_of_path, std::back_inserter(temp));
                    temp.erase(temp.begin()); // delte repeat road point
                    std::move(temp.rbegin(), temp.rend(), std::back_inserter(_result_path));
                    RoadInfo road_info_tmp;
                    road_info_tmp.edgeId = result_road_info.at(i + 1);
                    // road_info_tmp.direction = 1;
                    // if (road_info_tmp.edgeId < 0) {
                    //     road_info_tmp.edgeId = -1 * road_info.edgeId;
                    //     road_info_tmp.direction = 0;
                    // }
                    road_info_tmp.invalid = false;
                    _result_roadinfo.emplace_back(road_info_tmp);
                    i++;
                    continue;
                }
                //----------------------------------------------------------------------------------//

                //------------------------------- normal edge
                //--------------------------------------//
                if (_rng.getVertex(_rng.getEdge(result_road_info.at(i)).source).nodeID !=
                    _rng.getVertex(_rng.getEdge(result_road_info.at(i - 1)).target).nodeID) {
                    AERROR << " global path is not continus "
                           << "\n";
                    return rtvalue;
                }

                std::copy(edgepoint_ll.begin(), edgepoint_ll.end(), std::back_inserter(_result_path));
                //----------------------------------------------------------------------------------//
            }
            // ====================== finish merger point
            // =====================================//
            rtvalue = 0;
            return rtvalue;
        }

        int UpdateNode(const std::shared_ptr<DPNodeMessage>& msg) {
            int rtvalue = -1;
            if (msg == nullptr) {
                AERROR << " DPNodeMessage msg is nulptr" << '\n';
                return rtvalue;
            }
            if (msg->cur_node_ == nullptr) {
                AERROR << " DPNodeMessage msg -> cur_node_ is nulptr" << '\n';
                return rtvalue;
            }
            if (msg->prev_nodes_.empty()) {
                AERROR << " DPNodeMessage msg pre_nodes_ is empty" << '\n';
                return rtvalue;
            }

            std::vector<OSMId> result_road_info;

            for (auto& prev_node : msg->prev_nodes_) {
                _source_id = prev_node.id_;
                _target_id = msg->cur_node_->id_;

                OSMId start_node_id = prev_node.node_id_;
                OSMId end_node_id = msg->cur_node_->node_id_;

                int rtv_flag = SearchPath(result_road_info, start_node_id, end_node_id); //

                //======================== calc cost =================================//
                double cost = 0;
                if (rtv_flag == -1) {
                    cost = INFINITY;
                    msg->cur_node_->UpdateFailedEdge(_source_id);
                } else {
                    for (size_t i = 0; i < result_road_info.size(); ++i) {
                        if (i == 0) {
                            auto edge_points = _rng.getEdge(result_road_info.front()).EdgePoints;
                            auto len_of_edge = _rng.getEdge(result_road_info.front()).Length;
                            auto iter = std::find(edge_points.begin(), edge_points.end(), start_node_id);

                            auto distance = std::distance(iter, edge_points.end());
                            cost += len_of_edge * (std::fabs(distance) / edge_points.size());
                            continue;
                        }
                        if (i == result_road_info.size() - 1) {
                            auto edge_points = _rng.getEdge(result_road_info.back()).EdgePoints;
                            auto len_of_edge = _rng.getEdge(result_road_info.back()).Length;
                            auto iter = std::find(edge_points.begin(), edge_points.end(), end_node_id);

                            auto distance = std::distance(edge_points.begin(), iter);

                            cost += len_of_edge * (std::fabs(distance) / edge_points.size());
                            continue;
                        }

                        cost += _rng.getEdge(result_road_info.at(i)).Length;
                    }
                }

                std::cout << "from  " << _source_id << " to " << _target_id << " cost = " << cost << " start node id "
                          << start_node_id << " end node id " << end_node_id << '\n';
                cost += prev_node.min_cost_;
                // ============================== finish calc
                // cost===============================//

                msg->cur_node_->UpdateCost(&prev_node, cost, result_road_info);

                result_road_info.clear();
            }
            rtvalue = 0;
            return rtvalue;
        }

        int FindBlockEdge(const latlon& ll, OSMId& node_id, OSMId& edge_id, bool& is_vertex) {
            int rtvalue = -1;
            // kd-tree optimize
            const size_t                    numResults = 1;
            size_t                          resIndex;
            double                          outDistSqr;
            double                          queryPt[2] = {ll.lat, ll.lon};
            nanoflann::KNNResultSet<double> resultSet(numResults);
            resultSet.init(&resIndex, &outDistSqr);
            index->findNeighbors(resultSet, queryPt, nanoflann::SearchParams());

            node_id = cloud.pts.at(resIndex).node_id_;
            edge_id = cloud.pts.at(resIndex).edge_id_;

            const double out_dist = Coord::ComputeDist(ll.lat, ll.lon, _db.nodeFromID(node_id)->getLat(),
                                                       _db.nodeFromID(node_id)->getLon());
            // 输入了错误的禁止点，距离graph的最近距离小于10m（可调）;
            if (out_dist > 30) {
                rtvalue = -1;
                AERROR << "[ERROR] : the block point is too far to graph" << '\n';
                return 1;
            }

            // 如果当前匹配点-禁止点距离vertex在一定范围内，认为是匹配点为该顶点
            const auto& ll_to_node_dist = Coord::ComputeDist(_db.nodeFromID(node_id)->getLat(),
                                                             _db.nodeFromID(node_id)->getLon(), ll.lat, ll.lon);

            std::multimap<double, OSMId> candiate_vertexs;
            std::multimap<double, OSMId> candiate_dist_to_vertexs;
            for (const auto& v : _rng.getVeterxs()) {
                double ll_to_vertex_dist = Coord::ComputeDist(ll.lat, ll.lon, _db.nodeFromID(v.first)->getLat(),
                                                              _db.nodeFromID(v.first)->getLon());

                candiate_dist_to_vertexs.insert(std::make_pair(ll_to_vertex_dist, v.first));
            }

            if (candiate_dist_to_vertexs.empty()) {
                return rtvalue;
            }

            if (ll_to_node_dist <= 2 && candiate_dist_to_vertexs.begin()->first <= 2.5) {
                std::cout << " block is nearset vertex" << '\n';
                std::cout << "block candiate vertex size " << candiate_dist_to_vertexs.size() << '\n';
                std::cout << "block is vertex " << '\n';

                is_vertex = true;
                node_id = candiate_dist_to_vertexs.begin()->second;
                std::cout << " vettex id  " << node_id << '\n';

                // if car in vertex, and detect enemy front car, replan, use yaw of car
                // to add block edge,todo
                if (ll.yaw_valid) {
                }
            } else if (ll_to_node_dist < 2 && candiate_dist_to_vertexs.begin()->first > 2.5) {
                std::cout << "block is on edge " << edge_id << '\n';
                is_vertex = false;
            } else {
                std::cout << " block set failed " << '\n';
                is_vertex = false;
            }
            rtvalue = 0;
            return rtvalue;
        }

        int ProcessJunctionPoint(const latlon& last_mission_poins, const latlon& next_mission_points,
                                 const OSMId& vertex_id, double& cost) {
            cost += Coord::ComputeDist(last_mission_poins.lat, last_mission_poins.lon,
                                       _db.nodeFromID(vertex_id)->getLat(), _db.nodeFromID(vertex_id)->getLon());

            cost += Coord::ComputeDist(next_mission_points.lat, next_mission_points.lon,
                                       _db.nodeFromID(vertex_id)->getLat(), _db.nodeFromID(vertex_id)->getLon());

            return 0;
        }

        int ProcessCandiateEdges(const std::vector<OSMId>& last_level_candiate_edges,
                                 const std::vector<OSMId>& cur_level_candiate_edges, const OSMId& last_node_id,
                                 const OSMId& cur_node_id, OSMId& porcessed_candiate_edge) {
            int rtvalue = -1;
            std::cout << " process candiate edges " << '\n';

            if (_rng.getVertex(last_node_id).nodeID != -9999 || _rng.getVertex(cur_node_id).nodeID != -9999) {
                AINFO << " last " << last_node_id << " cure id " << cur_node_id;
                std::cout << " last node and cur node both is vertex " << '\n';
                return 0;
            }
            // 如果上个点带航向，当前层不带航向
            if (last_level_candiate_edges.size() == 1 && cur_level_candiate_edges.size() > 1) {
                // note
                // if ()
                std::cout << " last 1 cur 2 ::: "
                          << " last node " << last_node_id << "  cur node od " << cur_node_id << '\n';
                for (const auto& e_cur : cur_level_candiate_edges) {
                    std::cout << "e last " << last_level_candiate_edges.back() << " e cur  " << e_cur << '\n';
                    // 如果上一层的lane与当前层的候选edge'相同，则返回2，将当前层加入合并至上一层,如果当前edge是回环路，则return3。多加一层与航向相同的edge
                    if (last_level_candiate_edges.back() == e_cur) {
                        if (IsPointBehindAnotherPoint(last_level_candiate_edges.back(), last_node_id, cur_node_id)) {
                            std::cout << " same edgge 22222222222222222222222222222" << '\n';
                            rtvalue = 2;
                            return rtvalue;
                        }
                    }
                }
                rtvalue = 0;
                return rtvalue;
            }
            // 如果当前层带航向，如果上一层也带航向，则不做处理，如果上一层不带航向，则判断是否有相同edge，返回1，清空上一层，将处理后的相同edge加入上一层，如果当前edge是回环路，则return3。多加一层，同上
            if (cur_level_candiate_edges.size() == 1) {
                std::cout << " is yaw valid same edge" << '\n';
                if (last_level_candiate_edges.size() == 1) {
                    // std::cout << " sssssssssssss all all 1 " << '\n';
                    if (cur_level_candiate_edges.back() == last_level_candiate_edges.back()) {
                        return 2;
                    } else {
                        return 0;
                    }
                } else {
                    for (const auto& e_last : last_level_candiate_edges) {
                        if (e_last == cur_level_candiate_edges.back()) {
                            porcessed_candiate_edge = cur_level_candiate_edges.back();
                            return 1;
                        }
                    }
                }
            }

            std::cout << " last node " << last_node_id << "  cur node od " << cur_node_id << '\n';

            // 上一层与当前层均不带航向，则判断是否有相同edge，有则根据任务Ian的先后顺序，判断edge方向
            std::multimap<double, OSMId> candiate_same_edges;
            for (const auto& e_last : last_level_candiate_edges) {
                for (const auto& e_cur : cur_level_candiate_edges) {
                    std::cout << "e last " << std::fabs(e_last) << " e cur  " << std::fabs(e_cur) << '\n';
                    if (e_last == e_cur) {
                        std::cout << " same edgge 1111111111111111111111111" << '\n';

                        if (IsPointBehindAnotherPoint(e_cur, last_node_id, cur_node_id)) {
                            porcessed_candiate_edge = e_cur;
                        } else {
                            porcessed_candiate_edge = -e_cur;
                        }
                        candiate_same_edges.insert(
                            std::make_pair(_rng.getEdge(porcessed_candiate_edge).Length, porcessed_candiate_edge));
                    }
                }
            }
            if (!candiate_same_edges.empty()) {
                porcessed_candiate_edge = candiate_same_edges.begin()->second;
                rtvalue = 1;
                return rtvalue;
            }

            rtvalue = 0;
            return rtvalue;
        }

        bool IsPointBehindAnotherPoint(const OSMId& same_edge, const OSMId& first_point, const OSMId& second_point) {
            const auto& edge_points = _rng.getEdge(same_edge).EdgePoints;
            const auto& iter_of_first = std::find(edge_points.begin(), edge_points.end(), first_point);

            const auto& iter_of_second = std::find(edge_points.begin(), edge_points.end(), second_point);

            if (std::distance(iter_of_first, iter_of_second) > 0) {
                return true;
            }
            return false;
        }

        int SelectHeadingEdge(OSMId& wait_for_select_edge, const OSMId& node_id, double standard_yaw) {
            auto edge_points = _rng.getEdge(wait_for_select_edge).EdgePoints;
            if (edge_points.empty()) {
                AERROR << "[ERROR] : cant get edgetpoint from edge " << wait_for_select_edge;
                return -1;
            }
            auto iter = std::find(edge_points.begin(), edge_points.end(), node_id);
            if (iter == edge_points.end()) {
                AERROR << "source node id :" << node_id << " is not in edge : " << wait_for_select_edge;
                return -1;
            }

            // 向前移动的迭代器，向前移动10个距离，然后计算航向角
            auto iter_move_forward = iter;
            auto iter_move_backward = iter;
            // 前后移动当前点的迭代器，后续计算 航向角选择起始edge
            if (std::distance(iter, edge_points.end()) < 11)
                iter_move_forward = std::prev(edge_points.end());
            else
                std::advance(iter_move_forward, 10); // todo

            if (std::distance(edge_points.begin(), iter) < 11)
                iter_move_backward = edge_points.begin();
            else
                std::advance(iter_move_backward, -10);

            double azi_forward = Coord::ComputeAzimuth(_db.nodeFromID(*iter)->getLat(), _db.nodeFromID(*iter)->getLon(),
                                                       _db.nodeFromID(*iter_move_forward)->getLat(),
                                                       _db.nodeFromID(*iter_move_forward)->getLon());
            double azi_backward = Coord::ComputeAzimuth(
                _db.nodeFromID(*iter)->getLat(), _db.nodeFromID(*iter)->getLon(),
                _db.nodeFromID(*iter_move_backward)->getLat(), _db.nodeFromID(*iter_move_backward)->getLon());

            std::cout << "azi forward : " << azi_forward << "  edge id " << wait_for_select_edge;
            std::cout << "azi backward : " << azi_backward << "  edge id " << -wait_for_select_edge;
            double yaw = standard_yaw;
            AINFO << " mission point  yaw before normal  " << yaw;

            if (yaw < 0) {
                yaw += 360;
            }
            AINFO << " mission point  yaw after normal  " << yaw;

            if (std::fabs(azi_forward - yaw) > std::fabs(azi_backward - yaw))
                wait_for_select_edge = -wait_for_select_edge;

            return 0;
        }

        inline void QuadrantChecker(double& wait_for_check_yaw) {
            wait_for_check_yaw = std::fabs(wait_for_check_yaw - 360) < wait_for_check_yaw
                                     ? std::fabs(wait_for_check_yaw - 360)
                                     : wait_for_check_yaw;
        }

    private:
        struct Point {
            double lat_, lon_;
            OSMId  node_id_, edge_id_;

            Point() = default;

            Point(double lat, double lon, OSMId node_id, OSMId edge_id)
                : lat_(lat), lon_(lon), node_id_(node_id), edge_id_(edge_id) {};
        };

        struct PointCloud {
            std::vector<Point> pts;

            inline size_t kdtree_get_point_count() const {
                return pts.size();
            }

            inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
                if (dim >= 2) {
                    std::cerr << "Error! dim.\n";
                    exit(EXIT_FAILURE);
                }

                if (dim == 0)
                    return pts[idx].lat_;

                return pts[idx].lon_;
            }

            template <class BBOX> bool kdtree_get_bbox(BBOX&) const {
                return false;
            }
        };

        typedef nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L2_Simple_Adaptor<double, PointCloud>, PointCloud,
                                                           2>
            kdTree;

        kdTree*    index;
        PointCloud cloud;

        std::shared_ptr<AstarBase> _astar_base;
        OSMDataBase&               _db = OSMDataBase::Instance();
        RoadGraph&                 _rng = RoadGraph::Instance();

        std::vector<latlon> _points_ll;

        OSMId _source_id; // 传入A*的起始节点 -edge
        OSMId _target_id; // 传入A*的目标节点 -edge

        std::vector<latlon>   _result_path;
        std::vector<RoadInfo> _result_roadinfo;

        std::vector<OSMId> candiate_nodes_;

        latlon _start_ll;
        latlon _end_ll;
        OSMId  _start_id;
        OSMId  _end_id;

        std::list<std::list<DPNode>> _graph_nodes;

        std::vector<std::tuple<OSMId, OSMId, bool, bool>> _source_target_vec;
    };
} // namespace mapplan
#endif // OSM_OSMASTAR_H
