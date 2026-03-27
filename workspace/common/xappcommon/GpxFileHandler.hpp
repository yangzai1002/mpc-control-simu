#ifndef COMM_UTILS_GPX_FILE_UTILS_HPP
#define COMM_UTILS_GPX_FILE_UTILS_HPP

#include <glog/logging.h>
#include <openssl/md5.h>
#include <tinyxml2.h>

#include <atomic>
#include <filesystem>
#include <fstream>
#include <regex>
#include <vector>

#include "Datetime19Util.hpp"
#include "GeoUtils.hpp"
#include "HashUtils.hpp"
#include "JsonUtils.hpp"
#include "Waypoint.h"
#include "log/log.h"

namespace app_common {
    class GpxFileHandler {
    public:
        GpxFileHandler(const std::string& storing_dir, const std::string& holding_file)
            : _gpx_file_store_dir(storing_dir), _gpx_file_holding_using_gpx(holding_file) {
            std::error_code ec;
            std::filesystem::create_directories(_gpx_file_store_dir, ec);

            if (!std::filesystem::exists(holding_file)) {
                std::ofstream os(holding_file);
                os << "" << std::endl;
            }
        }

        GpxFileHandler(const GpxFileHandler&) = default;

        struct GpxFile {
            std::string   filename;
            std::int32_t  size;
            std::uint32_t update_time;
            std::string   md5sum;
        };

    public:
        bool gpx_file_using(const std::string& name) const {
            auto f = std::filesystem::path(name).replace_extension("").string();

            return f == using_gpx_file();
        }

        std::string using_gpx_file() const {
            std::ifstream is(_gpx_file_holding_using_gpx);

            std::string gpx_file_name;
            std::getline(is, gpx_file_name);

            return gpx_file_name;
        }

        void using_gpx_file(const std::string& name) const {
            auto p = file_path(name);
            if (!std::filesystem::exists(p)) {
                throw std::runtime_error("文件不存在，文件名: " + p.string());
            }

            // TODO: 文件锁
            std::ofstream os(_gpx_file_holding_using_gpx);

            std::string realname;
            if (GPX_FILE_EXTENSION == std::filesystem::path(name).extension()) {
                realname = std::filesystem::path(name).replace_extension("").string();
            } else {
                realname = name;
            }

            os << realname << std::endl;
        }

        std::string using_gpx_file_path() const {
            return file_path(using_gpx_file()).string();
        }

        std::string using_gpx_file_content() const {
            return gpx_file_content(using_gpx_file());
        }

        void using_gpx_file_content(const std::string& content) const {
            gpx_file_content(using_gpx_file(), content);
        }

        std::pair<std::string, std::string> using_gpx_file_start_end() const {
            return gpx_file_start_end(using_gpx_file());
        }

        bool gpx_file_exist(const std::string& name) const {
            auto p = file_path(name);
            return std::filesystem::exists(p);
        }

        std::size_t gpx_file_size(const std::string& name) const {
            auto p = file_path(name);
            return std::filesystem::file_size(p);
        }

        std::string gpx_file_content(const std::string& name) const {
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

        void gpx_file_content(const std::string& name, const std::string& content) const {
            auto p = file_path(name);

            AINFO << "filepath: " << p;
            std::ofstream os(p);
            if (!os.is_open()) {
                throw std::runtime_error("文件写入失败，文件名: " + p.string());
            }
            os << content;
        }

        void gpx_file_content(const std::string& name, tinyxml2::XMLDocument& doc) const {
            auto p = file_path(name);
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("文件写入失败，文件名: " + p.string());
            }
        }

        std::pair<std::string, std::string> gpx_file_start_end(const std::string& name) const {
            auto p = file_path(name);

            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(p.c_str())) {
                throw std::runtime_error("文件打开失败，文件名: " + p.string());
            }
            auto root = doc.RootElement();
            auto metadata = root->FirstChildElement("metadata");
            if (nullptr == metadata) {
                throw std::runtime_error("文件格式错误，文件名: " + p.string());
            }

            auto start = metadata->FirstChildElement("start");
            auto end = metadata->FirstChildElement("end");
            if (nullptr == start || nullptr == end) {
                throw std::runtime_error("文件格式错误，文件名: " + p.string());
            }

            auto start_name = std::string(start->GetText());
            auto end_name = std::string(end->GetText());

            return std::pair<std::string, std::string>{std::move(start_name), std::move(end_name)};
        }

        void gpx_file_start_end(const std::string& old_name, const std::string& new_name, const std::string& start_name,
                                const std::string& end_name) const {
            auto old_p = file_path(old_name);
            auto new_p = file_path(new_name);

            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(old_p.c_str())) {
                throw std::runtime_error("文件不存在，文件名: " + old_p.string());
            }
            auto root = doc.RootElement();
            auto metadata = root->FirstChildElement("metadata");
            if (nullptr == metadata) {
                throw std::runtime_error("文件格式错误，文件名: " + old_p.string());
            }
            auto name = metadata->FirstChildElement("name");
            auto start = metadata->FirstChildElement("start");
            auto end = metadata->FirstChildElement("end");
            if (nullptr == name || nullptr == start || nullptr == end) {
                throw std::runtime_error("文件格式错误，文件名: " + old_p.string());
            }
            name->SetText(new_name.c_str());
            start->SetText(start_name.c_str());
            end->SetText(end_name.c_str());

            if (tinyxml2::XML_SUCCESS != doc.SaveFile(old_p.c_str())) {
                throw std::runtime_error("文件保存失败，文件名: " + old_p.string());
            }

            std::filesystem::rename(old_p, new_p);
        }

        void gpx_file_length(tinyxml2::XMLDocument& doc, double length) const {
            auto root = doc.RootElement();
            auto metadata = root->FirstChildElement("metadata");
            if (nullptr == metadata) {
                throw std::runtime_error("XML格式错误");
            }
            auto element_length = metadata->FirstChildElement("length");
            if (nullptr == element_length) {
                element_length = doc.NewElement("length");
                if (nullptr == element_length) {
                    throw std::runtime_error("XML创建新元素失败");
                }
            }
            element_length->SetText(length);
            metadata->InsertEndChild(element_length);
        }

        void gpx_file_length(const std::string& name, double length) const {
            auto                  p = file_path(name);
            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.LoadFile(p.c_str())) {
                throw std::runtime_error("文件不存在，文件名: " + p.string());
            }
            gpx_file_length(doc, length);

            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("文件保存失败，文件名: " + p.string());
            }
        }

        std::vector<GpxFile> gpx_files() const {
            std::vector<GpxFile> items;
            for (auto& entry : std::filesystem::directory_iterator(_gpx_file_store_dir)) {
                const auto& path = entry.path();
                if (!std::filesystem::is_regular_file(path)) {
                    AINFO << path.string() << " is not a regular file";
                    continue;
                }

                auto ext = std::filesystem::path(path).extension();
                if (ext != GPX_FILE_EXTENSION) {
                    AINFO << path.string() << " is not end with .gpx";
                    continue;
                }
                GpxFile item;
                item.filename = path.filename().string();
                item.size = std::filesystem::file_size(path);
                item.update_time = std::filesystem::last_write_time(path).time_since_epoch().count();
                item.md5sum = HashUtils::MD5FromFile(path.string());

                AINFO << "gpx file: " << path.string() << ", size: " << item.size
                      << ", update_time: " << item.update_time;
                items.push_back(item);
            }
            std::sort(items.begin(), items.end(),
                      [](const GpxFile& a, const GpxFile& b) { return a.update_time > b.update_time; });

            return items;
        }

        void gpx_file_remove(const std::string& filename) const {
            auto p = file_path(filename);
            AINFO << "delete file: " << p;
            std::filesystem::remove(p);
        }

        void gpx_file_remove(const std::list<std::string>& filenames) const {
            for (auto filename : filenames) {
                gpx_file_remove(filename);
            }
        }

        void gpx_file_new(tinyxml2::XMLDocument& doc, const std::string& road_name, const std::string& road_start,
                          const std::string& road_end, bool interpolated = false) const {
            doc.Clear();
            auto declaration = doc.NewDeclaration();
            doc.InsertFirstChild(declaration);

            auto name = doc.NewElement("name");
            auto ctime = doc.NewElement("ctime");
            auto start = doc.NewElement("start");
            auto end = doc.NewElement("end");
            auto metadata = doc.NewElement("metadata");
            auto gpx = doc.NewElement("gpx");
            auto trk = doc.NewElement("trk");
            auto map_trkseg_ = doc.NewElement("trkseg");
            auto element_interpolated = doc.NewElement("interpolated");
            if (nullptr == name || nullptr == ctime || nullptr == start || nullptr == end || nullptr == metadata ||
                nullptr == gpx || nullptr == trk || nullptr == map_trkseg_ || nullptr == element_interpolated) {
                throw std::runtime_error("XML创建框架失败");
            }

            name->SetText(road_name.c_str());
            ctime->SetText(Datetime19Util::current_yyyymmddhhmiss().c_str());
            start->SetText(road_start.c_str());
            end->SetText(road_end.c_str());
            element_interpolated->SetText(interpolated);

            metadata->InsertEndChild(name);
            metadata->InsertEndChild(ctime);
            metadata->InsertEndChild(start);
            metadata->InsertEndChild(end);
            metadata->InsertEndChild(element_interpolated);

            trk->InsertEndChild(map_trkseg_);

            gpx->InsertFirstChild(metadata);
            gpx->SetAttribute("version", 1.0f);
            gpx->InsertEndChild(trk);

            doc.InsertEndChild(gpx);
        }

        void gpx_file_new(const std::string& road_name, const std::string& road_start, const std::string& road_end,
                          bool interpolated = false) const {
            tinyxml2::XMLDocument doc;

            gpx_file_new(doc, road_name, road_start, road_end, interpolated);

            auto p = file_path(road_name);
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("创建文件失败，文件名: " + p.string());
            }
        }

        void gpx_file_new(const std::string& road_name, const std::string& road_start, const std::string& road_end,
                          const std::vector<Waypoint>& waypoints, bool interpolated = false) const {
            tinyxml2::XMLDocument doc;

            gpx_file_new(doc, road_name, road_start, road_end, interpolated);

            tinyxml2::XMLElement* gpx = nullptr;
            tinyxml2::XMLElement* trk = nullptr;
            tinyxml2::XMLElement* trkseg = nullptr;

            if (nullptr == (gpx = doc.FirstChildElement("gpx")) || nullptr == (trk = gpx->FirstChildElement("trk")) ||
                nullptr == (trkseg = trk->FirstChildElement("trkseg"))) {
                throw std::runtime_error("文件格式错误，文件名: " + road_name);
            }

            for (const auto& wp : waypoints) {
                append(doc, trkseg, wp);
            }

            auto p = file_path(road_name);
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("保存文件失败，文件名: " + p.string());
            }
        }

        void gpx_file_new(const std::string& road_name, const std::string& road_start, const std::string& road_end,
                          const std::vector<Waypoint>& waypoints, const std::vector<Waypoint>& plan_hints,
                          bool interpolated = false) const {
            tinyxml2::XMLDocument doc;

            gpx_file_new(doc, road_name, road_start, road_end, interpolated);

            tinyxml2::XMLElement* gpx = nullptr;
            tinyxml2::XMLElement* trk = nullptr;
            tinyxml2::XMLElement* trkseg = nullptr;

            if (nullptr == (gpx = doc.FirstChildElement("gpx")) || nullptr == (trk = gpx->FirstChildElement("trk")) ||
                nullptr == (trkseg = trk->FirstChildElement("trkseg"))) {
                throw std::runtime_error("文件格式错误，文件名: " + road_name);
            }

            for (const auto& wp : waypoints) {
                AINFO << "---------------------------------- lat: " << std::setprecision(12) << wp.lat
                      << ", lon: " << wp.lon;
                append(doc, trkseg, wp);
            }

            auto element_hints = doc.NewElement("plan_hints");
            if (nullptr == element_hints) {
                throw std::runtime_error("增加文件节点失败");
            }
            for (const auto& wp : plan_hints) {
                append(doc, element_hints, wp);
            }
            gpx->InsertFirstChild(element_hints);

            auto p = file_path(road_name);
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("保存文件失败，文件名: " + p.string());
            }
        }

        std::pair<std::vector<Waypoint>, bool> gpx_file_to_waypoints(const std::string& name) const {
            auto p = file_path(name);

            tinyxml2::XMLDocument doc;
            if (doc.LoadFile(p.c_str()) != tinyxml2::XML_SUCCESS) {
                throw std::runtime_error("文件读取失败，文件名: " + p.string());
            }

            tinyxml2::XMLElement* gpx = nullptr;
            tinyxml2::XMLElement* metadata = nullptr;
            tinyxml2::XMLElement* start = nullptr;
            tinyxml2::XMLElement* end = nullptr;
            tinyxml2::XMLElement* trk = nullptr;
            tinyxml2::XMLElement* trkseg = nullptr;
            if ((nullptr == (gpx = doc.FirstChildElement("gpx"))) ||
                (nullptr == (metadata = gpx->FirstChildElement("metadata"))) ||
                (nullptr == (start = metadata->FirstChildElement("start"))) ||
                (nullptr == (end = metadata->FirstChildElement("end"))) ||
                (nullptr == (trk = gpx->FirstChildElement("trk"))) ||
                (nullptr == (trkseg = trk->FirstChildElement("trkseg")))) {
                throw std::runtime_error("文件格式错误，文件名: " + p.string());
            }

            std::vector<Waypoint> tmp;
            auto                  trkpt = trkseg->FirstChildElement();
            while (trkpt != nullptr) {
                Waypoint wp;
                wp.lat = trkpt->DoubleAttribute("lat");
                wp.lon = trkpt->DoubleAttribute("lon");
                wp.speed = trkpt->FloatAttribute("speed");
                wp.dis_start = trkpt->DoubleAttribute("dis_start");
                wp.fence_dis = trkpt->DoubleAttribute("fence_dis");
                tmp.push_back(wp);
                trkpt = trkpt->NextSiblingElement();
            }

            auto interpolated = metadata->FirstChildElement("interpolated");
            return {std::move(tmp), !(nullptr == interpolated || !interpolated->BoolText())};
        }

        std::vector<Waypoint> gpx_file_to_waypoints_interpolated(const std::string& name) const {
            auto p = file_path(name);

            tinyxml2::XMLDocument doc;
            if (doc.LoadFile(p.c_str()) != tinyxml2::XML_SUCCESS) {
                throw std::runtime_error("文件读取失败，文件名: " + p.string());
            }

            tinyxml2::XMLElement* gpx = nullptr;
            tinyxml2::XMLElement* metadata = nullptr;
            tinyxml2::XMLElement* start = nullptr;
            tinyxml2::XMLElement* end = nullptr;
            tinyxml2::XMLElement* trk = nullptr;
            tinyxml2::XMLElement* trkseg = nullptr;
            if ((nullptr == (gpx = doc.FirstChildElement("gpx"))) ||
                (nullptr == (metadata = gpx->FirstChildElement("metadata"))) ||
                (nullptr == (start = metadata->FirstChildElement("start"))) ||
                (nullptr == (end = metadata->FirstChildElement("end"))) ||
                (nullptr == (trk = gpx->FirstChildElement("trk"))) ||
                (nullptr == (trkseg = trk->FirstChildElement("trkseg")))) {
                throw std::runtime_error("文件格式错误，文件名: " + p.string());
            }

            std::vector<Waypoint> tmp;
            auto                  trkpt = trkseg->FirstChildElement();
            while (trkpt != nullptr) {
                Waypoint wp;
                wp.lat = trkpt->DoubleAttribute("lat");
                wp.lon = trkpt->DoubleAttribute("lon");
                wp.speed = trkpt->FloatAttribute("speed");
                wp.dis_start = trkpt->DoubleAttribute("dis_start");
                wp.fence_dis = trkpt->DoubleAttribute("fence_dis");
                tmp.push_back(wp);
                if (abs(wp.dis_start) <= EPSINON) {
                    GeoUtils::update_length_to_start_point(tmp);
                    trkpt->SetAttribute("dis_start", tmp.back().dis_start);
                }
                trkpt = trkpt->NextSiblingElement();
            }

            AINFO << "gpx file size: " << tmp.size();

            // 对地图文件做了修改，保存
            if (tinyxml2::XML_SUCCESS != doc.SaveFile(p.c_str())) {
                throw std::runtime_error("保存文件失败，文件名: " + p.string());
            }

            std::vector<Waypoint> points;
            // 北京与仿真联调时增加：如果是已经加密过的跕，则不再做加密操作
            auto interpolated = metadata->FirstChildElement("interpolated");
            if (nullptr == interpolated || !interpolated->BoolText()) {
                AINFO << "--gpx file size: " << tmp.size();
                GeoUtils::WaypointsInterpolation(tmp, points);
                AINFO << "--gpx file size: " << tmp.size();
                AINFO << "--gpx file size: " << points.size();
            } else {
                points = tmp;
            }
            AINFO << "gpx file size: " << points.size();
            return points;
        }

        void gpx_file_delete_segment(const std::string& name, const std::string& segment) const {
            auto                  content = gpx_file_content(name);
            tinyxml2::XMLDocument doc;
            if (tinyxml2::XML_SUCCESS != doc.Parse(content.c_str())) {
                throw std::runtime_error("文件内容解析失败，文件名: " + name);
            }
            tinyxml2::XMLElement* gpx = nullptr;
            tinyxml2::XMLElement* trk = nullptr;
            if (nullptr == (gpx = doc.FirstChildElement("gpx")) || nullptr == (trk = gpx->FirstChildElement("trk"))) {
                throw std::runtime_error("文件格式错误，文件名: " + name);
            }

            auto trkseg = trk->FirstChildElement("trkseg");
            while (trkseg != nullptr) {
                auto next = trkseg->NextSiblingElement();

                auto att = trkseg->Attribute("name");
                if (att != nullptr && att == segment) {
                    trk->DeleteChild(trkseg);
                }

                trkseg = next;
            }

            gpx_file_content(name, doc);
        }

        // TODO: 文件锁
    private:
        std::filesystem::path file_path(const std::string& name) const {
            std::filesystem::path p(_gpx_file_store_dir);
            std::string           realname;
            if (GPX_FILE_EXTENSION == std::filesystem::path(name).extension()) {
                realname = name;
            } else {
                realname = name + GPX_FILE_EXTENSION;
            }
            p.append(realname);
            return p;
        }

        void append(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* trkseg, const Waypoint& wp) const {
            if (nullptr == trkseg) {
                return;
            }

            auto trkpt = doc.NewElement("trkpt");
            if (nullptr == trkpt) {
                throw std::runtime_error("追加路径点失败");
            }
            std::ostringstream lat_os;
            lat_os << std::fixed << std::setprecision(12) << wp.lat;
            std::ostringstream lon_os;
            lon_os << std::fixed << std::setprecision(12) << wp.lon;
            trkpt->SetAttribute("lat", lat_os.str().c_str());
            trkpt->SetAttribute("lon", lon_os.str().c_str());
            trkpt->SetAttribute("speed", 20);
            trkpt->SetAttribute("dis_start", 0);
            trkpt->SetAttribute("fence_dis", 0);
            trkseg->InsertEndChild(trkpt);
        }

    private:
        const std::string _gpx_file_store_dir;
        const std::string _gpx_file_holding_using_gpx;

        static const constexpr double EPSINON = 0.00001;
        static const constexpr char*  GPX_FILE_EXTENSION = ".gpx";
    };
} // namespace app_common
#endif
