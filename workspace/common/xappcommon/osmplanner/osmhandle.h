#pragma once

#include "OSMDataBase.h"
#include "OSMParser.h"
#include "RoadGraph.h"
#include "faulthandle_api.hpp"
#include "filterkey.h"

namespace mapplan {
    class OSMHandle {
    public:
        OSMHandle() = default;
        ~OSMHandle() = default;

        void Init(const char* osmPath) {
            osm_file_ = osmPath;

            FilterKey::addWayFilterKey("oneway");
            FilterKey::addWayFilterKey("highway");
            FilterKey::addWayFilterKey("name");
            FilterKey::addNodeFilterKey("junction");
            FilterKey::addNodeFilterKey("end");
        }

        void Reset() {
            db_.Reset();
            rng_.Reset();
        }

        int parse() {
            int rtvalue = -1;
            //=====================解析osm文件==================//
            OSMParser parser(osm_file_);

            if (parser.Parse() == -1) {
                std::cout << "osm map file parse failed: " << osm_file_ << std::endl;
                FaultHandle::FaultApi::Instance()->SetFaultCode("4062001", "osm文件不存在或解析失败");
                return rtvalue;
            }
            FaultHandle::FaultApi::Instance()->ResetFaultCode("4062001");

            std::cout << "osm map file parse successfull" << '\n';

            //=================构建路网=======================//
            if (rng_.BuildGraph() == -1) {
                std::cout << "[ERROR] : road net build failed " << '\n';
                FaultHandle::FaultApi::Instance()->SetFaultCode("4062002", "基于osm文件拓扑关系构建失败");
                return rtvalue;
            }
            FaultHandle::FaultApi::Instance()->ResetFaultCode("4062002");

            std::cout << "road net build seccessfull" << '\n';
            rtvalue = 0;
            return rtvalue;
        }

    private:
        const char*  osm_file_;
        OSMDataBase& db_ = OSMDataBase::Instance();
        RoadGraph&   rng_ = RoadGraph::Instance();
    };
} // namespace mapplan