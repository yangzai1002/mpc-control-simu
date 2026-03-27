#ifndef DIR_UTILS_HPP
#define DIR_UTILS_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace app_common {
    class DirUtils {
    public:
        DirUtils() = delete;

        static bool is_sub_path(const std::filesystem::path& base, const std::filesystem::path& candidate) {
            auto b = base.lexically_normal();
            auto c = candidate.lexically_normal();

            // base 成分不能比 candidate 深
            auto bIt = b.begin(), cIt = c.begin();
            for (; bIt != b.end() && cIt != c.end(); ++bIt, ++cIt) {
                if (*bIt != *cIt)
                    return false;
            }
            return bIt == b.end(); // candidate 至少包含 base
        }

        static std::pair<bool, std::string> validate_path_against_whitelist(const std::filesystem::path& path) {
            if (path.empty()) {
                return {false, "Empty path"};
            }

            std::filesystem::path user_canonical_path;
            try {
                if (std::filesystem::exists(path)) {
                    user_canonical_path = std::filesystem::canonical(path);
                } else { // 如果路径不存在，使用父目录的 canonical，再拼回最后一节，避免 ../ 绕过
                    auto parent = path.parent_path();
                    if (parent.empty()) {
                        parent = std::filesystem::current_path();
                    }
                    auto parent_canonical = std::filesystem::canonical(parent);
                    user_canonical_path = parent_canonical / path.filename();
                }
            } catch (const std::filesystem::filesystem_error& e) {
                return {false, std::string("Error resolving path: ") + e.what()};
            } catch (...) {
                return {false, "Unknown error resolving user path"};
            }

            // 逐个检查白名单条目
            for (const auto& w : whitelist) {
                std::filesystem::path white_canonical;
                try { // 尝试 canonical 白名单项（如果白名单配置的是不存在路径，也支持以原始值比较）
                    if (std::filesystem::exists(w)) {
                        white_canonical = std::filesystem::canonical(w);
                    } else { // 尝试弱规范化
                        white_canonical = std::filesystem::weakly_canonical(w);
                    }
                } catch (...) { // 规范失败则退回原始 w（保守但允许配置不存在目录作为白名单——不过一般不推荐）
                    white_canonical = w;
                }

                if (is_sub_path(white_canonical, user_canonical_path)) {
                    return {true, ""}; // 路径在白名单内或等于它，通过验证
                }
            }

            return {false, "Path is not inside any whitelist directory: " + user_canonical_path.string()};
        }

    private:
        static constexpr const char* const whitelist[] = {"/opt/usr/"};
    };
} // namespace app_common

#endif