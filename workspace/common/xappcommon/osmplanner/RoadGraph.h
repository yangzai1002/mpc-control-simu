#ifndef _ROADGRAPH_
#define _ROADGRAPH_

#include <iostream>
// #include <algorithm>

#include "OSMDataBase.h"
#include "SubGraph.h"

namespace mapplan {

    class RoadGraph {
    public:
        static RoadGraph& Instance() {
            static RoadGraph inst;
            return inst;
        }

    public:
        ~RoadGraph() = default;

        int BuildGraph() {
            int rtvalue = -1;
            if (AddNodeToGraph() == -1) {
                std::cout << "[ERROR] : add node failed " << '\n';
                return rtvalue;
            }
            AddWayToGraph();
            AddInOutEdge();
            if (AddSubGraph() == -1) {
                std::cout << "[ERROR] : Add Subgraph failed" << '\n';
                return rtvalue;
            }
            CheckIsEnd();
            rtvalue = 0;
            return rtvalue;
        }

        int AddForbiddenArea(const OSMId& no_pass_road, bool is_vertex, bool is_replan_flag) {
            if (is_vertex) {
                if (Vertexs_.find(no_pass_road) == Vertexs_.end()) {
                    return -1;
                }

                for (const auto& e : Vertexs_.at(no_pass_road).out_edges) {
                    if (Edges_.find(e) == Edges_.end()) {
                        return -1;
                    }
                    no_pass_road_.emplace_back(e);
                    no_pass_road_.emplace_back(-e);
                    Edges_.at(e).visited = false;
                    Edges_.at(-e).visited = false;
                }
                return 0;
            }
            //如果是阻塞重规划触发，则只将当前车辆方向的路段加入禁止通行路段，需保证传入路段号方向正确
            if (is_replan_flag) {
                if (Edges_.find(no_pass_road) == Edges_.end())
                    return -1;
                no_pass_road_.emplace_back(no_pass_road);
                Edges_.at(no_pass_road).visited = false;
                return 0;
            }

            if (Edges_.find(no_pass_road) == Edges_.end()) {
                return -1;
            }
            no_pass_road_.emplace_back(no_pass_road);
            Edges_.at(no_pass_road).visited = false;
            Edges_.at(-no_pass_road).visited = false;

            return 0;
        }

        int RemoveForbiddenArea(const std::vector<OSMId>& remove_no_pass_road) {
            int rtvalue = -1;
            if (remove_no_pass_road.empty())
                return rtvalue;

            for (const auto& r : remove_no_pass_road) {
                const auto iter = std::find(no_pass_road_.begin(), no_pass_road_.end(), r);
                if (iter == no_pass_road_.end())
                    continue;
                no_pass_road_.erase(iter);
            }
            // visted_ true;
            for (const auto& r : remove_no_pass_road) {
                if (Edges_.find(r) == Edges_.end())
                    continue;

                if (!Edges_.at(r).visited) {
                    Edges_.at(r).visited = true;
                }
            }
            rtvalue = 0;
            return rtvalue;
        }

        void CleanForbiddenArea() {
            for (const auto& r : no_pass_road_) {
                if (Edges_.at(r).isOneWay) {
                    Edges_.at(r).visited = true;
                    continue;
                }
                Edges_.at(r).visited = true;
                Edges_.at(-r).visited = true;
            }
            no_pass_road_.clear();
        }

        const std::unordered_map<OSMId, GraphEdge>& getEdges() const {
            return Edges_;
        }

        const std::unordered_map<OSMId, Vertex>& getVeterxs() const {
            return Vertexs_;
        }

        GraphEdge getEdge(OSMId id) const {
            if (Edges_.find(id) != Edges_.end())
                return Edges_.at(id);
            GraphEdge empty;
            return empty;
        }

        Vertex getVertex(OSMId id) const {
            if (Vertexs_.find(id) != Vertexs_.end())
                return Vertexs_.at(id);
            Vertex empty;
            return empty;
        }

        void Reset() {
            subgraphs_.clear();
            Vertexs_.clear();
            Edges_.clear();
            LinkEdges_.clear();
            no_pass_road_.clear();
        }

    private:
        int AddNodeToGraph() {
            if (_db.getWays().empty()) {
                std::cout << "osm way is empty !" << '\n';
                return -1;
            }

            Vertex vtx_source, vtx_taget;
            for (const auto& w : _db.getWays()) {
                if (w->getNdRefs().empty()) {
                    std::cout << " edge " << w->getId() << " ndrefs is empy" << '\n';
                    return -1;
                }
                //         todo 回环路
                if (w->isClosed()) {
                    vtx_source.nodeID = w->getNdRefs().front();
                    Vertexs_.insert(std::make_pair(vtx_source.nodeID, vtx_source));
                    continue;
                }
                // mei yi duan way de qianhou duan dian ji wei dang qian graph zhong de
                // dingdian
                vtx_source.nodeID = w->getNdRefs().front();
                vtx_taget.nodeID = w->getNdRefs().back();

                Vertexs_.insert(std::make_pair(vtx_source.nodeID, vtx_source));
                Vertexs_.insert(std::make_pair(vtx_taget.nodeID, vtx_taget));
            }
            return 0;
        }

        int AddWayToGraph() {
            for (const auto& w : _db.getWays()) {
                std::string name;
                if (isOneWay(_db.wayFromID(w->getId()))) {
                    if (isLink(w, name)) {
                        addOneWayLinkEdge(w, name);
                        continue;
                    }
                    addOneWayEdge(w);
                    continue;
                }
                if (isLink(w, name)) {
                    addBiWayLinkEdges(w, name);
                    continue;
                }
                addBiWayEdges(w);
            }
            return 0;
        }

        int AddSubGraph() {
            int rtvalue = -1;
            if (LinkEdges_.empty()) {
                std::cout << "[INFO] : No SubGraph to add!" << '\n';
                rtvalue = 1;
                return rtvalue;
            }

            for (const auto& le : LinkEdges_) {
                std::vector<GraphEdge> in_edges;
                std::vector<GraphEdge> out_edges;

                Vertex vtp;
                vtp.nodeID = le.second.front().source; // use first link way id as vertex id
                // std::cout << "sub graph vertex id " << vtp.nodeID <<'\n';
                for (const auto& v : le.second) {
                    if (Vertexs_.find(v.source) == Vertexs_.end())
                        continue;

                    for (const auto& ie : Vertexs_.at(v.source).in_edges) {
                        vtp.in_edges.emplace_back(ie);
                        in_edges.emplace_back(Edges_.at(ie));
                        Edges_.at(ie).target = vtp.nodeID;
                    }
                    for (const auto& io : Vertexs_.at(v.source).out_edges) {
                        vtp.out_edges.emplace_back(io);
                        out_edges.emplace_back(Edges_.at(io));
                        Edges_.at(io).source = vtp.nodeID;
                    }
                    Vertexs_.erase(v.source);
                }
                std::shared_ptr<SubGraph> sg = std::make_shared<SubGraph>();
                if (sg->BuildSubGraph(le.second, in_edges, out_edges) == -1)
                    return rtvalue;
                vtp.SubGraph_ = sg;
                Vertexs_.insert(std::make_pair(vtp.nodeID, vtp));
                subgraphs_.insert(std::make_pair(vtp.nodeID, std::move(sg)));
            }
            rtvalue = 0;
            return rtvalue;
        }

        bool isOneWay(const Way& w) {
            bool is_one_way = false;
            for (const auto t : w->getTags()) {
                if (_db.getWayTagTable().getKey(t.first) == "oneway")
                    is_one_way = true;
            }
            return is_one_way;
        }

        bool isLink(const Way& w, std::string& name) {
            bool is_link = false;
            for (const auto t : w->getTags()) {
                if (_db.getWayTagTable().getKey(t.first) == "highway" &&
                    _db.getWayTagTable().getValuve(t.second) == "link")
                    is_link = true;
                if (_db.getWayTagTable().getKey(t.first) == "name")
                    name = _db.getWayTagTable().getValuve(t.second);
            }

            return is_link;
        }

        bool isInLinkEdges(const OSMId& id, OSMId& subgraph_vertex) {
            auto work = [&](const GraphEdge& e) -> bool { return (e.wayID == id); };
            for (auto& le : LinkEdges_) {
                if (std::any_of(le.second.begin(), le.second.end(), work)) {
                    if (subgraphs_.find(le.second.front().source) == subgraphs_.end())
                        return false;
                    subgraph_vertex = le.second.front().source;
                    return true;
                }
            }
            return false;
        }

        void AddInOutEdge() {
            std::cout << "Vertexs_:" << Vertexs_.size() << std::endl;
            if (Vertexs_.empty()) {
                std::cout << "map is wrong: Vertexs doesnot exist!" << std::endl;
                return;
            }
            for (const auto& e : Edges_) {
                if (Vertexs_.find(e.second.source) == Vertexs_.end())
                    std::cout << "map is wrong:Vertex doesnot exist in Way_1:" << e.second.source << std::endl;
                else
                    Vertexs_.at(e.second.source).out_edges.emplace_back(e.first);

                if (Vertexs_.find(e.second.target) == Vertexs_.end()) {
                    std::cout << "map is wrong:Vertex doesnot exist in Way_2:" << e.second.target << std::endl;
                    std::cout << "Element Not Present\n";
                } else
                    Vertexs_.at(e.second.target).in_edges.emplace_back(e.first);
            }
        }

        void addLinkEdgeToWay() {
            for (const auto& le : LinkEdges_) {
                for (const auto& e : le.second) {
                    Edges_.insert(std::make_pair(e.wayID, e));
                }
            }
        }

        void addOneWayEdge(const Way& w) {
            GraphEdge edge_tmp(w->getId(), w->getNdRefs(), w->getNdRefs().front(), w->getNdRefs().back(), w->Length(),
                               true);
            edge_tmp.visited = true;
            AddLLToEdge(w->getNdRefs(), edge_tmp.edgepoints_ll);
            Edges_.insert(std::make_pair(w->getId(), edge_tmp));
        }

        void addBiWayEdges(const Way& w) {
            GraphEdge edge_tmp_1(w->getId(), w->getNdRefs(), w->getNdRefs().front(), w->getNdRefs().back(), w->Length(),
                                 false);
            edge_tmp_1.direction = 1;
            edge_tmp_1.visited = true;
            AddLLToEdge(w->getNdRefs(), edge_tmp_1.edgepoints_ll);

            std::vector<OSMId> reversePoints = w->getNdRefs();
            std::reverse(reversePoints.begin(), reversePoints.end());
            GraphEdge edge_tmp_2(-w->getId(), reversePoints, w->getNdRefs().back(), w->getNdRefs().front(), w->Length(),
                                 false);
            edge_tmp_2.direction = -1;
            edge_tmp_2.visited = true;
            AddLLToEdge(reversePoints, edge_tmp_2.edgepoints_ll);
            Edges_.insert(std::make_pair(w->getId(), edge_tmp_1));
            Edges_.insert(std::make_pair(-w->getId(), edge_tmp_2));
        }

        void addBiWayLinkEdges(const Way& w, std::string& name) {
            if (LinkEdges_.find(name) == LinkEdges_.end()) {
                std::vector<GraphEdge> le;
                GraphEdge edge_tmp_1(w->getId(), w->getNdRefs(), w->getNdRefs().front(), w->getNdRefs().back(),
                                     w->Length(), false);
                edge_tmp_1.direction = 1;
                edge_tmp_1.visited = true;
                AddLLToEdge(w->getNdRefs(), edge_tmp_1.edgepoints_ll);

                std::vector<OSMId> reversePoints = w->getNdRefs();
                std::reverse(reversePoints.begin(), reversePoints.end());
                GraphEdge edge_tmp_2(-w->getId(), reversePoints, w->getNdRefs().back(), w->getNdRefs().front(),
                                     w->Length(), false);
                edge_tmp_2.direction = -1;
                edge_tmp_2.visited = true;
                AddLLToEdge(reversePoints, edge_tmp_2.edgepoints_ll);
                le.emplace_back(edge_tmp_1);
                le.emplace_back(edge_tmp_2);
                LinkEdges_.insert(std::make_pair(name, le));
                return;
            }
            GraphEdge edge_tmp_1(w->getId(), w->getNdRefs(), w->getNdRefs().front(), w->getNdRefs().back(), w->Length(),
                                 false);
            edge_tmp_1.direction = 1;
            edge_tmp_1.visited = true;
            AddLLToEdge(w->getNdRefs(), edge_tmp_1.edgepoints_ll);

            std::vector<OSMId> reversePoints = w->getNdRefs();
            std::reverse(reversePoints.begin(), reversePoints.end());
            GraphEdge edge_tmp_2(-w->getId(), reversePoints, w->getNdRefs().back(), w->getNdRefs().front(), w->Length(),
                                 false);
            edge_tmp_2.direction = -1;
            edge_tmp_2.visited = true;
            AddLLToEdge(reversePoints, edge_tmp_2.edgepoints_ll);
            LinkEdges_.at(name).emplace_back(edge_tmp_1);
            LinkEdges_.at(name).emplace_back(edge_tmp_2);
        }

        void addOneWayLinkEdge(const Way& w, std::string& name) {
            //如果当前name不在map中则新建
            if (LinkEdges_.find(name) == LinkEdges_.end()) {
                std::vector<GraphEdge> le;
                GraphEdge edge_tmp(w->getId(), w->getNdRefs(), w->getNdRefs().front(), w->getNdRefs().back(),
                                   w->Length(), true);
                edge_tmp.visited = true;
                AddLLToEdge(w->getNdRefs(), edge_tmp.edgepoints_ll);
                le.emplace_back(edge_tmp);
                LinkEdges_.insert(std::make_pair(name, le));
                return;
            }
            GraphEdge edge_tmp(w->getId(), w->getNdRefs(), w->getNdRefs().front(), w->getNdRefs().back(), w->Length(),
                               true);
            edge_tmp.visited = true;
            AddLLToEdge(w->getNdRefs(), edge_tmp.edgepoints_ll);
            LinkEdges_.at(name).emplace_back(edge_tmp);
        }
        void AddLLToEdge(const std::vector<OSMId>& ndrefs, std::vector<latlon>& edgepoints_ll) {
            latlon ll;
            for (const auto& n : ndrefs) {
                ll.lat = _db.nodeFromID(n)->getLat();
                ll.lon = _db.nodeFromID(n)->getLon();
                edgepoints_ll.emplace_back(ll);
            }
        }
        void CheckIsEnd() {
            for (auto& vtx : Vertexs_) {
                if (vtx.second.in_edges.size() == 1 & vtx.second.out_edges.size() == 1)
                    Edges_.at(vtx.second.in_edges.front()).is_dead_end = true;
            }
        }

    private:
        OSMDataBase&                                         _db = OSMDataBase::Instance();
        std::unordered_map<OSMId, std::shared_ptr<SubGraph>> subgraphs_;
        // 定点的id与定点
        std::unordered_map<OSMId, Vertex> Vertexs_;
        // edge primary way
        std::unordered_map<OSMId, GraphEdge> Edges_;
        // edge link way
        std::unordered_map<std::string, std::vector<GraphEdge>> LinkEdges_;

        std::vector<OSMId> no_pass_road_;

        RoadGraph() = default;

        RoadGraph(const RoadGraph&) = delete;

        RoadGraph(RoadGraph&&) = delete;
    };
} // namespace mapplan
#endif
