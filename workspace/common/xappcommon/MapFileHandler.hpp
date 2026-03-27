#ifndef MAP_FILE_HANDLER_HPP
#define MAP_FILE_HANDLER_HPP

#include <glog/logging.h>
#include <openssl/md5.h>
#include <tinyxml2.h>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>

#include "JsonUtils.hpp"
#include "Waypoint.h"

namespace app_common {
    class MapFileHandler {
    public:
        MapFileHandler(const std::string& base_dir) : MAP_FILE_DIR(base_dir) {
            std::error_code ec;
            std::filesystem::create_directories(MAP_FILE_DIR, ec);
            if (!std::filesystem::exists(MAP_USING_FILE_STORED_IN)) {
                std::ofstream os(MAP_USING_FILE_STORED_IN);
                os << "" << std::endl;
            }
        }

        struct MapFile {
            std::string   filename;
            std::int32_t  size;
            std::uint32_t update_time;
        };
        // TODO: 文件锁
    private:
        static const constexpr double EPSINON = 0.00001;

        const std::string MAP_FILE_DIR;

        static const constexpr char* MAP_USING_FILE_STORED_IN = "default_map.txt";

        static const constexpr char* MAP_FILE_EXTENSION = ".osm";

    private:
        std::filesystem::path file_path(const std::string& name) {
            std::filesystem::path p(MAP_FILE_DIR);
            std::string           realname;
            if (MAP_FILE_EXTENSION == std::filesystem::path(name).extension()) {
                realname = name;
            } else {
                realname = name + MAP_FILE_EXTENSION;
            }
            p.append(realname);
            return p;
        }

    public:
        bool map_file_using(const std::string& name) {
            auto f = std::filesystem::path(name).replace_extension("").string();

            return f == using_map_file();
        }

        std::string using_map_file() {
            std::ifstream is(MAP_USING_FILE_STORED_IN);

            std::string map_file_name;
            std::getline(is, map_file_name);

            return map_file_name;
        }

        void using_map_file(const std::string& name) {
            auto p = file_path(name);
            if (!std::filesystem::exists(p)) {
                throw std::runtime_error("路网文件不存在");
            }

            // TODO: 文件锁
            std::ofstream os(MAP_USING_FILE_STORED_IN);

            std::string realname;
            if (MAP_FILE_EXTENSION == std::filesystem::path(name).extension()) {
                realname = std::filesystem::path(name).replace_extension("").string();
            } else {
                realname = name;
            }

            os << realname << std::endl;
        }

        std::string using_map_file_content() {
            return map_file_content(using_map_file());
        }

        void using_map_file_content(const std::string& content) {
            map_file_content(using_map_file(), content);
        }

        bool map_file_exist(const std::string& name) {
            auto p = file_path(name);
            return std::filesystem::exists(p);
        }

        void map_file_load(const std::string& name, tinyxml2::XMLDocument& doc) {
            auto p = file_path(name);
            if (doc.LoadFile(p.c_str()) != tinyxml2::XML_SUCCESS) {
                throw std::runtime_error("文件加载失败，文件名: " + p.string());
            }
        }

        std::string map_file_content(const std::string& name) {
            auto          p = file_path(name);
            std::ifstream is(p);
            if (!is.is_open()) {
                throw std::runtime_error("文件打开失败，文件名: " + p.string());
            }
            std::ostringstream sstr;
            sstr << is.rdbuf();
            is.close();
            return sstr.str();
        }

        void map_file_content(const std::string& name, const std::string& content) {
            auto p = file_path(name);

            std::ofstream os(p);
            if (!os.is_open()) {
                throw std::runtime_error("文件写入失败，文件名: " + p.string());
            }
            os << content;
        }

        void map_file_content(const std::string& name, tinyxml2::XMLDocument& doc) {
            auto p = file_path(name);
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("文件写入失败，文件名: " + p.string());
            }
        }

        void map_file_content(const std::string& name, const std::filesystem::path& src) {
            auto p = file_path(name);
            std::filesystem::copy_file(src, p, std::filesystem::copy_options::overwrite_existing);
        }

        std::vector<MapFile> map_files() {
            std::vector<MapFile> items;
            for (auto& entry : std::filesystem::directory_iterator(MAP_FILE_DIR)) {
                const auto& path = entry.path();
                if (!std::filesystem::is_regular_file(path)) {
                    continue;
                }

                auto ext = std::filesystem::path(path).extension();
                if (ext != MAP_FILE_EXTENSION) {
                    continue;
                }
                MapFile item;
                item.filename = path.filename().string();
                item.size = std::filesystem::file_size(path);
                item.update_time = std::filesystem::last_write_time(path).time_since_epoch().count();
                items.push_back(item);
            }
            std::sort(items.begin(), items.end(),
                      [](const MapFile& a, const MapFile& b) { return a.update_time > b.update_time; });

            return items;
        }

        void map_file_remove(const std::string& filename) {
            auto p = file_path(filename);
            std::filesystem::remove(p);
        }

        void map_file_remove(const std::list<std::string>& filenames) {
            for (auto filename : filenames) {
                map_file_remove(filename);
            }
        }

        void map_file_new(tinyxml2::XMLDocument& doc) {
            doc.Clear();
            auto declaration = doc.NewDeclaration();
            doc.InsertFirstChild(declaration);
            auto osm = doc.NewElement("osm");
            osm->SetAttribute("version", "0.6");

            doc.InsertEndChild(osm);
        }

        void map_file_new(const std::string& name) {
            tinyxml2::XMLDocument doc;

            map_file_new(doc);

            auto p = file_path(name);
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("文件创建失败，文件名: " + p.string());
            }
        }
    };
}; // namespace app_common

#endif
