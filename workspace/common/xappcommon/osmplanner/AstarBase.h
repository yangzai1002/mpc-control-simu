//
// Created by syn on 22-9-6.
//
#ifndef OSM_ASTARBASE_H
#define OSM_ASTARBASE_H

#include <glog/logging.h>

#include <list>
#include <queue>
#include <set>
#include <vector>

#include "OSMParser.h"
#include "RoadGraph.h"
#include "coord.h"

namespace mapplan {

    struct RoadInfo {
        OSMId edgeId;
        int   direction;
        bool  invalid = true;
        RoadInfo() {
            edgeId = -999;
            direction = -999;
        }
        void Reset() {
            edgeId = -999;
            direction = -999;
            invalid = true;
        }

        bool operator==(const RoadInfo& other) const {
            return (*this).edgeId == other.edgeId;
        }
    };

    struct Anode {
        OSMId  ID;
        Anode* parent = nullptr;

        double H = 0;
        double G = 0;

        OSMId edgeid; // used by search based vertex
        Anode() {
            ID = 0;
            parent = nullptr;
            H = 0;
            G = 0;
            edgeid = -1;
        }
        ~Anode() = default;
        using Ptr = Anode*;

        bool operator<(const Anode* node) const {
            // in order to let the top of priority queue is the smallest one!
            return H + G > node->G + node->H;
        }
    };

    struct AnodeCompare {
        bool operator()(const Anode* _1st, const Anode* _2nd) {
            return (_1st->H + _1st->G) > (_2nd->H + _2nd->G);
        }
    };

    class OSMAstar;

    class AstarBase {
    public:
        AstarBase() {
            _target_id = -9999;
        }

        ~AstarBase() = default;

        //基于给定的起点终点进行路径搜索 // getpart length
        int SearchBase(const OSMId& source, const OSMId& target, std::vector<OSMId>& result_road_info) {
            int rtvalue = -1;
            if (!_rng.getEdge(target).visited) {
                LOG(ERROR) << "target is not reachable ";
                return rtvalue;
            }
            if (!_rng.getEdge(source).visited) {
                LOG(ERROR) << " source edge cant start !";
                return rtvalue;
            }

            auto source_anode = new Anode;
            source_anode->ID = source;
            source_anode->G = _rng.getEdge(source).Length;
            _target_id = target;
            Clear();
            _open_set.emplace(0, source_anode);
            while (!_open_set.empty()) {
                Anode::Ptr res_point = IsInOpenList(_target_id);
                auto       current_anode = NextAnode();

                if (res_point != nullptr) {
                    ConstructPath(res_point, result_road_info);
                    Clear();
                    rtvalue = 0;
                    return rtvalue;
                }

                if (AddNeibors(current_anode) == -1)
                    return rtvalue;
            }
            // std::cout << "[ERROR] : open set is empty !" << '\n';
            Clear();
            return rtvalue;
        }

        static double SearchShortDist(OSMId source, OSMId target) {
            std::multimap<double, Anode::Ptr> open_set;
            std::list<Anode::Ptr>             close_list;
            std::unordered_map<OSMId, OSMId>  edge_vertex_map; // first = edgeid , second = vertex id ;

            auto& rng = RoadGraph::Instance();
            auto& db = OSMDataBase::Instance();

            auto clear = [&]() -> void {
                for (auto ptr : close_list)
                    delete ptr;
                for (auto ptr : open_set)
                    delete ptr.second;
                close_list.clear();
                open_set.clear();
            };

            auto current_node = new Anode;
            current_node->ID = source;
            open_set.insert(std::make_pair(0, current_node));

            while (!open_set.empty()) {
                current_node = open_set.begin()->second;
                close_list.emplace_back(current_node);
                open_set.erase(open_set.begin());

                // solved
                if (current_node->ID == target) {
                    clear();
                    // std::cout << " find target " <<'\n';
                    return current_node->G;
                }

                for (const auto& e : rng.getVertex(current_node->ID).out_edges) {
                    auto target_id = rng.getEdge(e).target;

                    edge_vertex_map.insert(std::make_pair(e, target_id));

                    if (std::any_of(close_list.begin(), close_list.end(),
                                    [&](const Anode::Ptr node) { return node->ID == target_id; })) {
                        continue;
                    } else if (std::any_of(open_set.begin(), open_set.end(),
                                           [&](const std::pair<double, Anode::Ptr>& open) {
                                               return open.second->ID == target_id;
                                           })) {
                        continue;
                    }

                    auto anode = new Anode;
                    anode->ID = target_id;
                    anode->parent = current_node;
                    anode->G = current_node->G + rng.getEdge(e).Length;
                    anode->H =
                        Coord::ComputeDist(db.nodeFromID(target_id)->getLat(), db.nodeFromID(target_id)->getLon(),
                                           db.nodeFromID(target)->getLat(), db.nodeFromID(target)->getLon());
                    open_set.insert(std::make_pair((anode->G + anode->H), anode));
                }
            }
            clear();
            std::cout << "[ERROR] : cant find from " << source << " to " << target << '\n';
            return -1;
        }

        int SearchBasedVertex(const OSMId& sourve_vertex, const OSMId& target_vertex, const OSMId& same_edge_id,
                              std::vector<OSMId>& result_road_info) {
            std::multimap<double, Anode::Ptr> open_map;
            std::unordered_set<OSMId>         close_set;

            // duantou lu
            const auto& out_edge = _rng.getVertex(target_vertex).out_edges;

            if (out_edge.empty()) {
                return -1;
            }
            if (out_edge.size() == 1 && std::fabs(_rng.getEdge(out_edge.back()).wayID) == std::fabs(same_edge_id)) {
                std::cout << " target_ vertex:  " << target_vertex << " is dead end" << '\n';
                return -1;
            }

            if (!_rng.getEdge(same_edge_id).visited) {
                std::cout << " same edge " << same_edge_id << " is blocked" << '\n';
                return -1;
            }

            auto current_node = new Anode;
            current_node->ID = sourve_vertex;
            current_node->G = 0;
            current_node->H = 0;
            open_map.insert(std::make_pair(0, current_node));

            size_t iter_num = 0;
            while (!open_map.empty()) {
                if (iter_num > 100) {
                    std::cout << "search iteration is satisy max iter num 100 " << '\n';

                    // clear
                    for (auto ptr : open_map)
                        delete ptr.second;
                    close_set.clear();
                    open_map.clear();
                    return -1;
                }

                current_node = open_map.begin()->second;

                if (current_node == nullptr || _rng.getVertex(current_node->ID).nodeID == -9999) {
                    std::cout << __FUNCTION__ << " " << __LINE__ << " curretn node is nullptr" << '\n';
                    return -1;
                }

                close_set.insert(current_node->ID);
                open_map.erase(open_map.begin());

                // solved
                if (current_node->ID == target_vertex) {
                    // trace back
                    auto anode_tmp = current_node;
                    result_road_info.emplace_back(same_edge_id);
                    while (anode_tmp->parent != nullptr) {
                        result_road_info.emplace_back(anode_tmp->edgeid);
                        anode_tmp = anode_tmp->parent;
                    }
                    result_road_info.emplace_back(same_edge_id);

                    std::cout << " find target " << '\n';

                    std::reverse(result_road_info.begin(), result_road_info.end());

                    // clear
                    for (auto ptr : open_map)
                        delete ptr.second;
                    close_set.clear();
                    open_map.clear();
                    return 0;
                }

                for (const auto& e : _rng.getVertex(current_node->ID).out_edges) {
                    auto neibor = _rng.getVertex(_rng.getEdge(e).target);
                    //             angle filter
                    const auto& edge_points_cur = _rng.getEdge(current_node->edgeid).edgepoints_ll;
                    const auto& edge_points_neibors = _rng.getEdge(e).edgepoints_ll;

                    double yaw = 180;
                    if (!edge_points_cur.empty() && !edge_points_neibors.empty()) {
                        yaw = std::fabs(Coord::ComputeAzimuth(edge_points_cur, false) -
                                        Coord::ComputeAzimuth(edge_points_neibors, true));
                    }
                    if (close_set.count(neibor.nodeID) > 0) {
                        continue;
                    }
                    if (std::any_of(open_map.begin(), open_map.end(), [&](const std::pair<double, Anode::Ptr>& op) {
                            return op.second->ID == current_node->ID;
                        })) {
                        continue;
                    }
                    if (std::fabs(e) == std::fabs(same_edge_id) || !_rng.getEdge(e).visited ||
                        std::fabs(e) == std::fabs(current_node->ID) || yaw < 30) {
                        continue;
                    }
                    auto anode = new Anode;
                    anode->ID = neibor.nodeID;
                    anode->G = _rng.getEdge(e).Length;
                    anode->H = Coord::ComputeDist(
                        _db.nodeFromID(neibor.nodeID)->getLat(), _db.nodeFromID(neibor.nodeID)->getLon(),
                        _db.nodeFromID(target_vertex)->getLat(), _db.nodeFromID(target_vertex)->getLon());
                    anode->edgeid = e;
                    anode->parent = current_node;
                    double cost = anode->G + anode->H;
                    open_map.insert(std::make_pair(cost, anode));
                }

                ++iter_num;
            }

            // clear
            for (auto ptr : open_map)
                delete ptr.second;
            close_set.clear();
            open_map.clear();
            std::cout << "[ERROR] : cant find from " << sourve_vertex << " to " << target_vertex << '\n';
            return -1;
        }

    private:
        int AddNeibors(Anode::Ptr current_node) {
            int rtvalue = -1;
            // first = edge id , second = target id
            std::vector<OSMId> neibors;
            //根据当前点=边的ID在图中找到该边对应的终点的出边，为该边的邻居边
            for (const auto& e : _rng.getVertex(_rng.getEdge(current_node->ID).target).out_edges) {
                neibors.emplace_back(e);
            }

            if (neibors.empty()) {
                // std::cout<< "[ERROR] : neibors is empty" << '\n';
                return rtvalue;
            }

            for (const auto e : neibors) {
                //如果当前node已经在closelist中或者当前路段被禁止通行或者当前道路为断头路并且该断头路不为终点
                // 不希望原路掉头 || std::fabs(e) == std::fabs(current_node->ID)
                //回环路
                // 角度过滤
                const auto& edge_points_cur = _rng.getEdge(current_node->ID).edgepoints_ll;
                const auto& edge_points_neibors = _rng.getEdge(e).edgepoints_ll;

                const auto& yaw = std::fabs(Coord::ComputeAzimuth(edge_points_cur, false) -
                                            Coord::ComputeAzimuth(edge_points_neibors, true));
                if (IsInCloseList(e)                                    //已经在close list中
                    || !_rng.getEdge(e).visited                         //被禁止通行
                    || (_rng.getEdge(e).is_dead_end && _target_id != e) //当前道路为断头路并且该断头路不为终点
                    || (std::fabs(current_node->ID) == std::fabs(e) &&
                        !_rng.getEdge(current_node->ID).is_dead_end) // 不希望原路掉头,当当前路段为断头路时允许原路掉头
                    || (_rng.getEdge(e).is_closed && e != _target_id) //会环路且当前邻接边不为终点
                    || (yaw < 10 &&
                        !_rng.getEdge(current_node->ID).is_dead_end) //角度过滤，当当前路段为断头路时忽视角度过滤
                                                                     ////根据车型确定是否开启
                ) {
                    continue;
                }

                auto res_point = IsInOpenList(e);
                if (IsInOpenList(e)) {
                    // cost not update todo
                    double G = _rng.getEdge(e).Length + current_node->G;
                    if (res_point->G > G) {
                        const auto& cost = res_point->G + res_point->H;
                        res_point->parent = current_node;
                        res_point->G = G;
                    }
                    continue;
                }

                auto anode = new Anode;
                anode->ID = e;
                anode->parent = current_node;
                anode->G = _rng.getEdge(e).Length + current_node->G;
                anode->H = Coord::ComputeDist(_db.nodeFromID(_rng.getEdge(e).target)->getLat(),
                                              _db.nodeFromID(_rng.getEdge(e).target)->getLon(),
                                              _db.nodeFromID(_rng.getEdge(_target_id).source)->getLat(),
                                              _db.nodeFromID(_rng.getEdge(_target_id).source)->getLon());
                const double cost = anode->G + anode->H;
                _open_set.insert(std::make_pair(cost, anode));
            }
            rtvalue = 0;
            return rtvalue;
        }

        Anode::Ptr NextAnode() {
            if (_open_set.empty())
                return nullptr;
            auto lower_cost_anode = _open_set.begin()->second;
            _close_list.emplace_back(lower_cost_anode);
            _open_set.erase(_open_set.begin());
            return lower_cost_anode;
        }

        int ConstructPath(Anode::Ptr target_node, std::vector<OSMId>& result_road_info) {
            auto anode_tmp = target_node;

            while (anode_tmp != nullptr) {
                result_road_info.emplace_back(_rng.getEdge(anode_tmp->ID).wayID);
                anode_tmp = anode_tmp->parent;
            }

            std::reverse(result_road_info.begin(), result_road_info.end());
            return 0;
        }

        void Clear() {
            for (auto ptr : _close_list)
                delete ptr;
            for (auto ptr : _open_set)
                delete ptr.second;
            _close_list.clear();
            _open_set.clear();
        }

        bool IsInCloseList(const OSMId& id) {
            if (_close_list.empty())
                return false;
            return std::any_of(_close_list.begin(), _close_list.end(),
                               [&](const Anode::Ptr close) { return close->ID == id; });
        }

        Anode::Ptr IsInOpenList(const OSMId& id) {
            if (_open_set.empty())
                return nullptr;
            for (const auto& e : _open_set) {
                if (id == e.second->ID)
                    return e.second;
            }
            return nullptr;
        }

    private:
        OSMDataBase& _db = OSMDataBase::Instance();
        RoadGraph&   _rng = RoadGraph::Instance();

        OSMId _target_id;

        std::list<Anode::Ptr>             _close_list;
        std::multimap<double, Anode::Ptr> _open_set;

        std::vector<RoadInfo> _result_roadinfo;
    };
} // namespace mapplan
#endif // OSM_ASTARBASE_H
