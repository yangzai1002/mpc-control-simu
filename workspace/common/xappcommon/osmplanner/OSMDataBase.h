#ifndef _OSMDATABASE_
#define _OSMDATABASE_

#include "OSMHeader.h"
#include "OSMNode.h"
#include "OSMRelation.h"
#include "OSMTagTable.h"
#include "OSMWay.h"

#include <assert.h>
#include <iostream>
#include <unordered_map>

namespace mapplan {
class OSMDataBase {
public:
    static OSMDataBase& Instance() {
        static OSMDataBase inst;
        return inst;
    }

public:
    void init(BOUNDS& bounds, Nodes& nodes, OSMTagTable& nodetagtable,
              Ways& ways, OSMTagTable& waytagtable, Relations& relations,
              OSMTagTable& relationtagtable) {

        bounds_ = bounds;
        nodes_ = std::move(nodes);
        nodeTagTable_ = std::move(nodeTagTable_);
        ways_ = std::move(ways);
        wayTagTable_ = std::move(waytagtable);
        relations_ = relations;
        relationTagTable_ = relationtagtable;

        createIdMap();
    }

    const BOUNDS& getBounds() const {
        return bounds_;
    }

    Nodes getNodes() const {
        Nodes empty;
        if (nodes_.empty())
            return empty;
        return nodes_;
    }

    const OSMTagTable& getNodeTagTable() const {
        return nodeTagTable_;
    }

    Ways getWays() const {
        Ways empty;
        if (ways_.empty())
            return empty;
        return ways_;
    }

    const OSMTagTable& getWayTagTable() const {
        return wayTagTable_;
    }

    const Relations& getRelations() const {
        return relations_;
    }

    const OSMTagTable& getRelationTagTable() const {
        return relationTagTable_;
    }

    Node nodeFromID(OSMId id) const {
        Node empty;
        if (idToNodeMap_.find(id) == idToNodeMap_.end())
            return empty;
        return idToNodeMap_.at(id);
    }

    Way wayFromID(OSMId id) const {
        Way empty;
        if (idToWayMap_.find(id) == idToWayMap_.end())
            return empty;
        return idToWayMap_.at(id);
    }

    const Relation& relationoFromID(OSMId id) const {
        return idToRelationMap_.at(id);
    }

    void Reset() {
        nodes_.clear();
        ways_.clear();
        relations_.clear();
        idToNodeMap_.clear();
        idToWayMap_.clear();
        idToRelationMap_.clear();
    }

private:
    // bian jie
    BOUNDS bounds_{};
    // all node in osm file
    Nodes nodes_;
    OSMTagTable
        nodeTagTable_; // 所有node具有的tag表
                       // ,根据node中的tag中的索引值，在表中索引相应的键值对。
    // all way in osm file
    Ways ways_;
    OSMTagTable wayTagTable_; // 所有way具有的tag表
    // all relation in osm file
    Relations relations_;
    OSMTagTable relationTagTable_; // 所有relation具有的tag表

    // osmid 对应的元素map
    std::unordered_map<OSMId, const Node> idToNodeMap_;
    std::unordered_map<OSMId, const Way> idToWayMap_;
    std::unordered_map<OSMId, const Relation> idToRelationMap_;

    OSMDataBase() = default;

    OSMDataBase(const OSMDataBase&) = delete;

    OSMDataBase(OSMDataBase&&) = delete;

    void createIdMap() {
        idToNodeMap_.clear();
        idToWayMap_.clear();
        idToRelationMap_.clear();

        idToNodeMap_.reserve(nodes_.size());
        idToWayMap_.reserve(ways_.size());
        idToRelationMap_.reserve(relations_.size());

        bool inserted;
        for (const auto& n : nodes_) {
            std::tie(std::ignore, inserted) =
                idToNodeMap_.insert(std::make_pair(n->getId(), n));
            assert(inserted);
        }

        for (const auto& w : ways_) {
            std::tie(std::ignore, inserted) =
                idToWayMap_.insert(std::make_pair(w->getId(), w));
            assert(inserted);
        }

        for (const auto& r : relations_) {
            std::tie(std::ignore, inserted) =
                idToRelationMap_.insert(std::make_pair(r->getId(), r));
            assert(inserted);
        }
    }
};
} // namespace mapplan
#endif //_OSMDATABASE_
