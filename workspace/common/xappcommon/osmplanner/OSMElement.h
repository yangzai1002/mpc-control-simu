#ifndef _OSMEMELEMNT_
#define _OSMEMELEMNT_

#include <utility>
#include <vector>

#include "OSMHeader.h"
namespace mapplan {
class OSMElement {
public:
    OSMElement() = default;
    OSMElement(const OSMElement&) = default;
    OSMElement& operator=(const OSMElement&) = default;
    virtual ~OSMElement() = default;

    void initF(OSMId id, std::vector<OSMTag> tags) {
        id_ = id;
        tags_ = std::move(tags);
    }

    OSMId getId() const {
        return id_;
    }

    std::vector<OSMTag> getTags() const {
        std::vector<OSMTag> empty;
        if (tags_.empty())
            return empty;
        return tags_;
    }

private:
    OSMId id_{}; // 元素的id
    // OSMElementType type_; // 元素类型
    std::vector<OSMTag> tags_; // 元素内含有的tags
};
} // namespace mapplan
#endif //_OSMEMELEMNT_