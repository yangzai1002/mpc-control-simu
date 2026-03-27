#pragma once

#include <map>
#include <memory>
#include <unordered_set>

#include "OSMDataBase.h"
#include "coord.h"

namespace mapplan {
    class SubGraph;

    struct GraphEdge;
    using CandiatePaths = std::multimap<double, std::pair<bool, std::vector<GraphEdge>>>;

    template <class T1, class T2> struct pair_hash {
        size_t operator()(const std::pair<T1, T2>& p) const {
            return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
        }

        bool operator()(const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const {
            return std::equal_to<T1>()(lhs.first, rhs.first) && std::equal_to<T2>()(lhs.second, rhs.second);
        }
    };

    struct GraphEdge {
        OSMId               wayID;
        std::vector<OSMId>  EdgePoints;
        std::vector<latlon> edgepoints_ll;
        OSMId               source;
        OSMId               target;

        double Length;
        int    direction = 1;
        bool   visited = true;
        bool   isOneWay = false;
        bool   is_closed = false;
        bool   is_dead_end = false;

        GraphEdge() {
            wayID = -9999;
            source = -9999;
            target = -9999;
            Length = 0;
        }

        GraphEdge(OSMId id, std::vector<OSMId> edgepoints, OSMId s, OSMId t, double len, bool isoneway)
            : wayID(id), EdgePoints(std::move(edgepoints)), source(s), target(t), Length(len), isOneWay(isoneway) {
        }
    };

    struct Vertex {
        OSMId              nodeID;
        std::vector<OSMId> in_edges;
        std::vector<OSMId> out_edges;
        // landmark dist
        mutable std::shared_ptr<std::map<OSMId, double>> dist_from_lmrk_avoid;
        mutable std::shared_ptr<std::map<OSMId, double>> dist_to_lmrk_avoid;

        std::shared_ptr<SubGraph> SubGraph_ = nullptr;
        bool                      is_end_ = false;

        Vertex() {
            nodeID = -9999;
        }

        void AddLandmarkCost(OSMId id, double cost, bool reverse = false) const {
            if (reverse) {
                if (dist_to_lmrk_avoid == nullptr) {
                    dist_to_lmrk_avoid = std::make_shared<std::map<OSMId, double>>();
                }
                (*dist_to_lmrk_avoid)[id] = cost;
                return;
            }
            if (dist_from_lmrk_avoid == nullptr) {
                dist_from_lmrk_avoid = std::make_shared<std::map<OSMId, double>>();
                ;
            }
            (*dist_from_lmrk_avoid)[id] = cost;
        }
    };

    class SubGraph {
    public:
        SubGraph() = default;

        ~SubGraph() = default;

        struct SubVertex {
            OSMId                  nodeID; //
            std::vector<GraphEdge> in_edges;
            std::vector<GraphEdge> out_edges;
            bool                   visted = false;
        };

        int BuildSubGraph(const std::vector<GraphEdge>& linkedge, const std::vector<GraphEdge>& in_edges,
                          const std::vector<GraphEdge>& out_edges) {
            int rtvalue = -1;
            if (linkedge.empty() || in_edges.empty() || out_edges.empty())
                return rtvalue;
            // build sub graph
            for (const auto& le : linkedge) {
                SubVertex sv;
                sv.nodeID = le.source;
                _sub_vertex.insert(std::make_pair(le.source, sv));
            }
            for (const auto& e : linkedge) {
                _sub_edges.insert(std::make_pair(std::make_pair(e.source, e.target), e));
                _sub_vertex.at(e.source).out_edges.emplace_back(e);
                _sub_vertex.at(e.target).in_edges.emplace_back(e);
            }
            // serach connect path
            std::unordered_set<std::pair<OSMId, OSMId>, pair_hash<OSMId, OSMId>> source_target_set;
            for (const auto& ie : in_edges) {
                _in_out_edges.insert(std::make_pair(ie.wayID, ie));
                for (const auto& oe : out_edges) {
                    if (ie.source == oe.target)
                        continue;
                    _in_out_edges.insert(std::make_pair(oe.wayID, oe));
                    source_target_set.insert(std::make_pair(ie.wayID, oe.wayID));
                }
            }
            for (const auto& st : source_target_set) {
                // std::cout << "se : " <<st.first << " oe :" <<st.second << '\n';
                CandiatePaths candiate_paths = DFS(st.first, st.second);
                if (candiate_paths.empty())
                    return rtvalue;
                _sub_path.insert(std::make_pair(st, candiate_paths));
            }
            ////    // print
            //    for (const auto& ft : _sub_path) {
            //        std::cout << "------------------------------" <<'\n';
            //        std::cout <<"map size ::  " <<ft.second.size() << '\n';
            //        std::cout << "form: "<< ft.first.first<< "  to:  "<<
            //        ft.first.second << '\n'; for (auto& t : ft.second) {
            //            std::cout <<"link edges： " ;
            //            for (const auto& r : t.second.second){
            //               std::cout << r.wayID << "  ";
            //            }
            //            std::cout <<'\n';
            //        }
            //        std::cout << "------------------------------" <<'\n';
            //    }
            // protect
            if (_sub_path.empty()) {
                std::cout << "[ERROR] : _sub_path is empty1" << '\n';
                return rtvalue;
            }
            rtvalue = 0;
            return rtvalue;
        }

        std::vector<GraphEdge> GetSubEdge(const OSMId& source, const OSMId& target) const {
            std::vector<GraphEdge> empty;
            auto                   st = std::make_pair(source, target);
            if (_sub_path.find(st) == _sub_path.end() || _sub_path.empty())
                return empty;
            for (const auto& path : _sub_path.at(st)) {
                if (!path.second.first)
                    continue;
                return path.second.second;
            }
            return empty;
        }

        void SetNoPassRoad(const OSMId& no_pass_road_id) {
            _no_pass_road.emplace_back(no_pass_road_id);

            auto work = [&](const GraphEdge& e) {
                bool rtvalue = false;
                if (e.wayID == no_pass_road_id || e.wayID == -no_pass_road_id)
                    rtvalue = true;
                return rtvalue;
            };

            for (auto& sp : _sub_path) {
                for (auto& mp : sp.second) {
                    if (std::any_of(mp.second.second.begin(), mp.second.second.end(), work))
                        mp.second.first = false;
                }
            }
        }

        void ResetNopassRoad(const OSMId& reset_no_pass_road_id) {
            if (_no_pass_road.empty())
                return;
            // delete reset_no_pass_road_id in _no_pass_road
            _no_pass_road.erase(std::find(_no_pass_road.begin(), _no_pass_road.end(), reset_no_pass_road_id));
            const auto iter = std::find(_no_pass_road.begin(), _no_pass_road.end(), -reset_no_pass_road_id);
            if (iter != _no_pass_road.end())
                _no_pass_road.erase(iter);

            auto work = [&](const GraphEdge& e) {
                bool rtvalue = false;
                if (e.wayID == reset_no_pass_road_id || e.wayID == -reset_no_pass_road_id)
                    rtvalue = true;
                return rtvalue;
            };
            for (auto& sp : _sub_path)
                for (auto& mp : sp.second)
                    if (std::any_of(mp.second.second.begin(), mp.second.second.end(), work))
                        for (const auto& r : mp.second.second)
                            if (!std::any_of(_no_pass_road.begin(), _no_pass_road.end(),
                                             [&](const OSMId& id) { return id == r.wayID; }))
                                mp.second.first = true;
        }

    private:
        CandiatePaths DFS(const OSMId& start, const OSMId& end) {
            OSMId source_node, target_node;
            source_node = _in_out_edges.at(start).target;
            target_node = _in_out_edges.at(end).source;

            std::vector<GraphEdge> path_tmp;
            CandiatePaths          candiate_paths;

            std::function<int(OSMId&, std::vector<GraphEdge>&, double&)> dfs =
                [&](const OSMId& node_id, std::vector<GraphEdge>& path_vec, double& length) -> int {
                _sub_vertex.at(node_id).visted = true;
                for (auto& ne : _sub_vertex.at(node_id).out_edges) {
                    std::vector<GraphEdge> path_tmp;
                    std::copy(path_vec.begin(), path_vec.end(), std::back_inserter(path_tmp));
                    //判断当前点是否被遍历过
                    if (_sub_vertex.at(ne.target).visted)
                        continue;
                    auto st = std::make_pair(node_id, ne.target);
                    length += ne.Length;
                    if (_sub_edges.find(st) == _sub_edges.end())
                        return -1;
                    path_tmp.emplace_back(_sub_edges.at(st));
                    if (ne.target == target_node) {
                        candiate_paths.insert(std::make_pair(length, std::make_pair(true, std::move(path_tmp))));
                        length = 0;
                        continue;
                    }
                    if (dfs(ne.target, path_tmp, length) == -1)
                        return -1;
                }
                _sub_vertex.at(node_id).visted = false;
                return 0;
            };

            double len = 0;
            if (dfs(source_node, path_tmp, len) == -1)
                candiate_paths.clear();
            return std::move(candiate_paths);
        }

        std::unordered_map<std::pair<OSMId, OSMId>, CandiatePaths, pair_hash<OSMId, OSMId>> _sub_path;

        std::unordered_map<std::pair<OSMId, OSMId>, GraphEdge, pair_hash<OSMId, OSMId>> _sub_edges;

        std::unordered_map<OSMId, GraphEdge> _in_out_edges;

        std::unordered_map<OSMId, SubVertex> _sub_vertex;
        std::vector<OSMId>                   _no_pass_road;
    };
} // namespace mapplan