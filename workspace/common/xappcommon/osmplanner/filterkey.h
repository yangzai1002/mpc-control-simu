#ifndef _FILTER_KEY_H_
#define _FILTER_KEY_H_

#include <string>
#include <vector>

class FilterKey {
public:
    static FilterKey &Instance(){
        static FilterKey instance;
        return instance;
    }

    static void addNodeFilterKey(const char* key) {
            Instance().keyLists4Node.emplace_back(key);
        }

        static void addWayFilterKey(const char* key) {
           Instance().keyLists4Way.emplace_back(key);
        }

 static void addRelationFilterKey(const char* key) {
           Instance().keyLists4Relation.emplace_back(key);
        }

        static std::vector<const char*> getNodeFilterKeys() {
            return Instance().keyLists4Node;
        }

        static std::vector<const char*> getWayFilterKeys() {
            return Instance().keyLists4Way;
        }

        static std::vector<const char*> getRelationFilterKeys() {
            return Instance().keyLists4Relation;
        }


private:
    FilterKey() = default;


private:
    std::vector<const char*> keyLists4Node;
    std::vector<const char*> keyLists4Way;
    std::vector<const char*> keyLists4Relation;
};

#endif
