#ifndef _OSMTAGTABLE_
#define _OSMTAGTABLE_

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "OSMHeader.h"

namespace mapplan {
class OSMTagTable {
public:
    // 返回值为当前添加的key在数组中的索引
    size_t addKey(const std::string& k) {
        Keys_.emplace_back(k);
        return Keys_.size() - 1;
    }

    size_t addValvue(const std::string& v) {
        Values_.emplace_back(v);
        return Values_.size() - 1;
    }

    const std::string& getKey(size_t idOfKey) const {
        return Keys_.at(idOfKey);
    }

    const std::string& getValuve(size_t idOfValuve) const {
        return Values_.at(idOfValuve);
    }

    size_t getIndexOfKey(const std::string& s) const {
        return std::find(Keys_.begin(), Keys_.end(), s) - Keys_.begin();
    }

    size_t getIndexOfValuve(const std::string& s) const {
        return std::find(Values_.begin(), Values_.end(), s) - Values_.begin();
    }

    bool isKeyValid(size_t index) const {
        return index < Keys_.size() - 1;
    }

    bool isValuveValid(size_t index) const {
        return index < Values_.size() - 1;
    }

    const std::vector<std::string>& getKeys() const {
        return Keys_;
    }

    const std::vector<std::string>& getValuves() const {
        return Values_;
    }

    //依据索引查找相应的键值对
    std::function<std::pair<const std::string&, const std::string&>(
        std::pair<size_t, size_t>)>
    pairLookup() const {
        return [this](const std::pair<size_t, size_t> p) {
            return std::make_pair(std::cref(Keys_.at(p.first)),
                                  std::cref(Values_.at(p.second)));
        };
    }

    // std::function<const std::string&(size_t) > valueLookup() const {
    //     return [this](size_t i) {
    //         return std::cref(Values_.at(i));
    //     };
    // }
    // //std::ref 用于包装按引用传递的值。
    // //std::cref 用于包装按const引用传递的值。
    // std::function<const std::string&(size_t) > keyLookup() const {
    //     return [this](size_t i) {
    //         return std::cref(Keys_.at(i));
    //     };
    // }

private:
    std::vector<std::string> Keys_;
    std::vector<std::string> Values_;
};
} // namespace mapplan
#endif //_OSMTAGTABLE_