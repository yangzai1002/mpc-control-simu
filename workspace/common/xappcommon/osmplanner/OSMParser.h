#ifndef _OSMPARSER_
#define _OSMPARSER_

#include <unistd.h>

#include "filterkey.h"
#include <cmath>
#include <iostream>
#include <map>

#include "OSMDataBase.h"
#include "coord.h"
#include "tinyxml2.h"

namespace mapplan {
    using namespace tinyxml2;

    class OSMParser {
    public:
        OSMParser() = delete;

        explicit OSMParser(const char* OSMFp) : OSMFilePath(OSMFp){};

        int Parse() {
            if (access(OSMFilePath, F_OK) == -1) {
                std::cout << "ERROR: Please check if the file in the path exists" << '\n';
                return -1;
            }
            XMLDocument osm_in;
            // 加载osm文件
            XMLError result = osm_in.LoadFile(OSMFilePath);

            if (result == XML_SUCCESS) {
                std::cout << " INFO: osm file load successful" << '\n';
            } else {
                std::cout << "INFO: osm file load failed: " << OSMFilePath << std::endl;
                return -1;
            }

            XMLElement* osm_root = nullptr; // osm 根节点
            XMLElement* node = nullptr;     // node节点
            XMLElement* way = nullptr;      // way节点
            XMLElement* relation = nullptr; // relation节点
            XMLElement* bounds = nullptr;   // bounds jiedian

            osm_root = osm_in.FirstChildElement();

            OSMTagTable nodeTT;
            OSMTagTable wayTT;
            OSMTagTable relationTT;

            Ways      db_ways;
            Nodes     db_nodes;
            Relations db_relations;
            BOUNDS    db_bounds;

            std::cout << "start parsing osm file" << '\n';
            parseNodes(node, osm_root, &nodeTT, &db_nodes);
            parseWays(way, osm_root, &wayTT, &db_ways);
            // parseRelations(relation, osm_root, &relationTT, &db_relations);
            // parseBounds(bounds, osm_root, &db_bounds);

            if (db_ways.empty()) {
                std::cout << "[WARNING] : db_ways is empty"
                          << "\n";
            }
            if (db_nodes.empty()) {
                std::cout << "[WARNING] : db_nodes is empty"
                          << "\n";
            }

            db_.init(db_bounds, db_nodes, nodeTT, db_ways, wayTT, db_relations, relationTT);

            if (addLengthTOWay() == -1) {
                std::cout << "[ERRROR] : add leghtn wo way failed" << '\n';
                return -1;
            }
            std::cout << "INFO: finish parsing osm file" << '\n';
            return 0;
        }

    private:
        void parseBounds(XMLElement* RootBound, XMLElement* RootOsm, BOUNDS* bounds) {
            RootBound = RootOsm->FirstChildElement("bounds");
            RootBound->QueryDoubleAttribute("minlat", &bounds->minlat);
            RootBound->QueryDoubleAttribute("minlon", &bounds->minlon);
            RootBound->QueryDoubleAttribute("maxlat", &bounds->maxlat);
            RootBound->QueryDoubleAttribute("maxlon", &bounds->maxlon);
        }

        void parseNodes(XMLElement* RootNode, XMLElement* RootOsm, OSMTagTable* osmTT, Nodes* nodes) {
            std::cout << "INFO: parsing osm node" << '\n';

            OSMId  id = 0; // osm node id;
            double lat = 0;
            double lon = 0;

            RootNode = RootOsm->FirstChildElement("node"); // get node

            while (RootNode != nullptr) {
                RootNode->QueryInt64Attribute("id", &id);
                RootNode->QueryDoubleAttribute("lat", &lat);
                RootNode->QueryDoubleAttribute("lon", &lon);

                auto action = RootNode->Attribute("action");
                if (action != nullptr && std::string("delete") == action) {
                    std::cout << "have node action is delete , id : " << id << std::endl;
                    RootNode = RootNode->NextSiblingElement("node"); // bianli node
                    continue;
                }

                // std::cout << id << '\n';
                XMLElement* tag = RootNode->FirstChildElement("tag");
                /// ToDo// filter tag key
                std::vector<OSMTag> tags_tmp;
                // jiexi tag kv
                parseTags(tag, &tags_tmp, osmTT, NODE);

                Node node_tmp = std::make_shared<OSMNode>(); // chuanjian shared ptr

                node_tmp->init(id, tags_tmp, lat, lon); // chushihua

                nodes->emplace_back(node_tmp);

                RootNode = RootNode->NextSiblingElement("node"); // bianli node
            }
        }

        void parseWays(XMLElement* RootWay, XMLElement* RootOsm, OSMTagTable* osmTT, Ways* ways) {
            std::cout << "INFO: parsing osm way" << '\n';

            OSMId ndRef = 0; // osm node id
            OSMId id = 0;    // way id

            RootWay = RootOsm->FirstChildElement("way");

            while (RootWay != nullptr) {
                RootWay->QueryInt64Attribute("id", &id);

                auto action = RootWay->Attribute("action");
                if (action != nullptr && "delete" == std::string(action)) {
                    std::cout << "have way action is delete , id : " << id << std::endl;
                    RootWay = RootWay->NextSiblingElement("way"); // bianli node
                    continue;
                }

                std::vector<OSMId> ndRefs_tmp;
                Way                way_tmp = std::make_shared<OSMWay>();

                // get nd refs
                XMLElement* nd = RootWay->FirstChildElement("nd");
                while (nd != nullptr) {
                    nd->QueryInt64Attribute("ref", &ndRef);

                    ndRefs_tmp.emplace_back(ndRef);
                    nd = nd->NextSiblingElement("nd");
                }

                // get tags
                std::vector<OSMTag> tags_tmp;
                XMLElement*         tag = RootWay->FirstChildElement("tag");
                parseTags(tag, &tags_tmp, osmTT, WAY);

                // chu shi hua way
                way_tmp->init(id, tags_tmp, ndRefs_tmp);

                ways->emplace_back(way_tmp);

                RootWay = RootWay->NextSiblingElement("way");
            }
        }

        void parseRelations(XMLElement* RootRelation, XMLElement* RootOsm, OSMTagTable* osmTT, Relations* relations) {
            std::cout << "INFO: parsing osm relation" << '\n';

            OSMId id = 0;
            RootRelation = RootOsm->FirstChildElement("relation");

            // get relation
            while (RootRelation != nullptr) {
                Relation relation_tmp = std::make_shared<OSMRelation>();

                RootRelation->QueryInt64Attribute("id", &id); // get relation id
                // get memebers
                XMLElement* member = RootRelation->FirstChildElement("member");

                OSMRelation::Member member_tmp{};

                while (member != nullptr) {
                    std::string type;
                    auto        t = member->Attribute("type");
                    if (t != nullptr) {
                        type = t;
                    }
                    member_tmp.id = member->Int64Attribute("ref");
                    member_tmp.role = member->Attribute("role");
                    if ("node" == type) {
                        member_tmp.type = NODE;
                    } else if ("way" == type) {
                        member_tmp.type = WAY;
                    } else {
                        member_tmp.type = RELATION;
                    }
                    member = member->NextSiblingElement("member");
                    relation_tmp->addMember(member_tmp);
                }

                // get tags
                std::vector<OSMTag> tags_tmp;
                XMLElement*         tag = RootRelation->FirstChildElement("tag");
                parseTags(tag, &tags_tmp, osmTT, RELATION);

                relation_tmp->initF(id, tags_tmp); // chishi hua id tags;
                relations->emplace_back(relation_tmp);

                RootRelation = RootRelation->NextSiblingElement("relation");
            }
        }

        bool filterKey(const char* key, OSMElementType type) {
            bool isKeyExist = false;
            switch (type) {
                case NODE:
                    for (auto k : FilterKey::getNodeFilterKeys()) {
                        if (strstr(k, key) != nullptr) {
                            isKeyExist = true;
                        }
                    }
                    break;
                case WAY:
                    for (auto k : FilterKey::getWayFilterKeys()) {
                        if (strstr(k, key) != nullptr) {
                            isKeyExist = true;
                        }
                    }
                    break;
                case RELATION:
                    for (auto k : FilterKey::getRelationFilterKeys()) {
                        if (strstr(k, key) != nullptr) {
                            isKeyExist = true;
                        }
                    }
                    break;
                case Invalid:
                    break;
            }
            return isKeyExist;
        }

        int addLengthTOWay() {
            for (const auto& w : db_.getWays()) {
                if (w->getNdRefs().empty()) {
                    std::cout << " edge ndrefs is empy" << '\n';
                    return -1;
                }
                std::unordered_map<OSMId, double> dists_tmp;
                OSMId                             preRef = 0;
                size_t                            i = 0;
                double                            length = 0;

                for (const auto& ref : w->getNdRefs()) {
                    if (i > 0) {
                        double dist =
                            Coord::ComputeDist(db_.nodeFromID(preRef)->getLat(), db_.nodeFromID(preRef)->getLon(),
                                               db_.nodeFromID(ref)->getLat(), db_.nodeFromID(ref)->getLon());
                        dists_tmp.insert(std::make_pair(preRef, dist));
                        length += dist;
                    }
                    preRef = ref;
                    ++i;
                }
                w->addDistToWay(dists_tmp);
                w->addLength(length);
            }
            return 0;
        }

        template <class T> void parseTags(XMLElement* RootTag, std::vector<OSMTag>* tags, OSMTagTable* osmTT, T t) {
            while (RootTag != nullptr) {
                OSMTag      tag_tmp;
                std::string key;
                std::string value;

                auto k = RootTag->Attribute("k"); // get tag key
                auto v = RootTag->Attribute("v"); // get tag value
                if (k != nullptr) {
                    key = k;
                }
                if (v != nullptr) {
                    value = v;
                }

                // std::cout<< key << '\n';
                if (filterKey(key.c_str(), t)) {
                    tag_tmp.first = osmTT->addKey(key);       // index of key in table
                    tag_tmp.second = osmTT->addValvue(value); // index of value in table
                    tags->emplace_back(tag_tmp);
                }
                RootTag = RootTag->NextSiblingElement("tag");
            }
        }

        const char*  OSMFilePath;
        OSMDataBase& db_ = OSMDataBase::Instance();
    };
} // namespace mapplan
#endif // _OSMPARSER_
