#ifndef _OSMNODE_
#define _OSMNODE_

#include <utility>

#include "OSMElement.h"
#include "filterkey.h"

namespace mapplan {
    class OSMNode : public OSMElement {
    public:
        OSMNode() = default;

        ~OSMNode() override = default;

        void init(OSMId id, std::vector<OSMTag>& tags, double lat, double lon) {
            lat_ = lat;
            lon_ = lon;
            OSMElement::initF(id, std::move(tags));
        }

        double getLat() const {
            return lat_;
        }

        double getLon() const {
            return lon_;
        }

    private:
        double lat_ = -999;
        double lon_ = -999;
    };
} // namespace mapplan
#endif // _OSMNODE_