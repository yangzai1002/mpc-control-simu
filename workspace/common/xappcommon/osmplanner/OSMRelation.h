#ifndef _OSMRELATION_
#define _OSMRELATION_

#include "OSMElement.h"

namespace mapplan {
    class OSMRelation : public OSMElement {
    public:
        struct Member {
            OSMId          id;
            OSMElementType type;
            const char*    role;
        };

        OSMRelation() = default;

        void addMember(const Member m) {
            members_.emplace_back(m);
        }

        void addMember(OSMId id, OSMElementType type, const char* role) {
            addMember(Member{id, type, role});
        }

        const std::vector<Member>& getMembers() const {
            return members_;
        }

    private:
        std::vector<Member> members_;
    };
} // namespace mapplan
#endif //_OSMRELATION_