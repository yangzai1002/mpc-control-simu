#ifndef _OSMWAY_
#define _OSMWAY_

#include <iostream>
#include <unordered_map>

#include "OSMElement.h"

namespace mapplan {
    class OSMWay : public OSMElement {
    public:
        OSMWay() = default;

        void init(OSMId id, const std::vector<OSMTag>& tags, const std::vector<OSMId>& ndrefs) {
            OSMElement::initF(id, tags);
            NdRefs_ = ndrefs;
        }

        const std::vector<OSMId>& getNdRefs() const {
            return NdRefs_;
        }

        bool isClosed() const {
            if (NdRefs_.empty()) {
                std::cout << " ndrefs is empty"
                          << "\n";
                return false;
            }
            return NdRefs_.front() == NdRefs_.back();
        }
        // 返回当前点到下一点的距离
        const double& Dist(OSMId id) const {
            return DistOfTwoPoint_.at(id);
        }
        //返回way的长度
        const double& Length() const {
            return length_;
        }

        int addDistToWay(std::unordered_map<OSMId, double>& dist) {
            DistOfTwoPoint_ = dist;
            return 0;
        }

        int addLength(double len) {
            length_ = len;
            return 0;
        }

    private:
        std::unordered_map<OSMId, double> DistOfTwoPoint_; // 当前点与下一点之间的距离长度

        double length_ = -999;

        std::vector<OSMId> NdRefs_;
    };
} // namespace mapplan
#endif //_OSMWAY_