#ifndef _OSMHEADER_
#define _OSMHEADER_

#include <memory>
#include <vector>

namespace mapplan {
enum OSMElementType { Invalid = 0, NODE, WAY, RELATION };

class OSMElement;
class OSMNode;
class OSMWay;
class OSMRelation;

typedef std::shared_ptr<OSMNode> Node;
typedef std::shared_ptr<OSMWay> Way;
typedef std::shared_ptr<OSMRelation> Relation;

typedef std::vector<Node> Nodes;
typedef std::vector<Way> Ways;
typedef std::vector<Relation> Relations;

typedef struct bounds {
    double minlat, minlon, maxlat, maxlon;
} BOUNDS;

typedef int64_t OSMId;

typedef std::pair<size_t, size_t> OSMTag;
} // namespace mapplan
#endif //_OSMHEADER_