#ifndef GPX_FILE_CONTROLLER_HPP
#define GPX_FILE_CONTROLLER_HPP

#include "GpxFileHandler.hpp"
#include "HashUtils.hpp"
#include "JsonUtils.hpp"

namespace app_common {
    class GpxFileController {
    public:
        GpxFileController(const std::string& dir, const std::string& default_file)
            : _gpx_file_handler(dir, default_file) {
        }

        virtual ~GpxFileController() = default;

        GpxFileHandler Handler() {
            return _gpx_file_handler;
        }

        void List(const nlohmann::json& req, nlohmann::json& res) {
            auto items = _gpx_file_handler.gpx_files();
            res["data"] = nlohmann::json::object({{"list", nlohmann::json::array()}});
            for (auto& item : items) {
                res["data"]["list"].push_back(nlohmann::json({{"filename", item.filename},
                                                              {"size", item.size},
                                                              {"update_time", item.update_time},
                                                              {"md5", item.md5sum}}));
            }
        }

        void Get(const nlohmann::json& req, nlohmann::json& res) {
            auto using_file = _gpx_file_handler.using_gpx_file();

            auto start_end = _gpx_file_handler.gpx_file_start_end(using_file);

            res["data"] = nlohmann::json::object(
                {{"road_name", using_file}, {"start_name", start_end.first}, {"end_name", start_end.second}});
        }

        void Set(const nlohmann::json& req, nlohmann::json& res) {
            std::string road_name;
            app_common::JsonUtils::valueFromJson(road_name, req, "road_name", true);

            res["data"] = nlohmann::json::object({});

            _gpx_file_handler.using_gpx_file(road_name);
        }

        void Download(const nlohmann::json& req, nlohmann::json& res) {
            std::string  filename;
            std::int16_t slice_index;
            std::int32_t slice_size;
            app_common::JsonUtils::valueFromJson(filename, req, "filename", true);
            app_common::JsonUtils::valueFromJson(slice_index, req, "slice_index", true);
            app_common::JsonUtils::valueFromJson(slice_size, req, "slice_size", true);
            if (filename.empty()) {
                throw std::runtime_error("请求文件名为空");
            }

            uint32_t           req_slice_size = 0 == slice_size ? UINT_MAX : slice_size;
            static std::string buffer;
            static std::string md5;
            static std::string buferr_filename;
            if (slice_index == 0) {
                buffer = _gpx_file_handler.gpx_file_content(filename);
                md5 = app_common::HashUtils::MD5(buffer);
                buferr_filename = filename;
            } else if (buferr_filename != filename) {
                throw std::runtime_error("当前请求的文件和初始化时不同");
            }

            int slice_total = buffer.size() / req_slice_size + 1;
            if (slice_index > slice_total - 1) {
                throw std::runtime_error("slice序号超出范围");
            }
            int remaining_size = buffer.size() - slice_index * slice_size;
            if (remaining_size > req_slice_size) {
                slice_size = req_slice_size;
            } else {
                slice_size = remaining_size;
            }

            res["data"] = nlohmann::json::object({{"filename", filename},
                                                  {"md5", md5},
                                                  {"size", buffer.size()},
                                                  {"real_file_size", buffer.size()},
                                                  {"slice_index", slice_index},
                                                  {"slice_total", slice_total},
                                                  {"slice_size", slice_size},
                                                  {"content", buffer.substr(slice_index * slice_size, slice_size)}});
        }

        void Upload(const nlohmann::json& req, nlohmann::json& res) {
            std::string  filename;
            std::string  md5;
            std::int32_t size;
            std::int16_t slice_index;
            std::int16_t slice_total;
            std::int32_t slice_size;
            std::string  content;

            app_common::JsonUtils::valueFromJson(filename, req, "filename", true);
            app_common::JsonUtils::valueFromJson(md5, req, "md5", true);
            app_common::JsonUtils::valueFromJson(size, req, "size", true);
            app_common::JsonUtils::valueFromJson(slice_index, req, "slice_index", true);
            app_common::JsonUtils::valueFromJson(slice_total, req, "slice_total", true);
            app_common::JsonUtils::valueFromJson(slice_size, req, "slice_size", true);
            app_common::JsonUtils::valueFromJson(content, req, "content", true);

            res["data"] = nlohmann::json::object({});

            static std::string buffer;
            static int         current_slice = 0;
            if (slice_index == 0) {
                current_slice = 0;
                buffer.clear();
            } else if (slice_index != current_slice) {
                current_slice = 0;
                buffer.clear();
                throw std::runtime_error("slice序号非递增");
            }
            buffer += content;
            current_slice += 1;
            if (md5 != app_common::HashUtils::MD5(content)) {
                throw std::runtime_error("文件MD5校验失败");
            }

            if (current_slice < slice_total) {
                return;
            }

            if (buffer.size() != size) {
                LOG(WARNING) << "file size not equal: req=" << size << ", buffer=" << buffer.size();
                throw std::runtime_error("文件大小不匹配");
            }

            _gpx_file_handler.gpx_file_content(filename, buffer);

            current_slice = 0;
            buffer.clear();
        }

        void Delete(const nlohmann::json& req, nlohmann::json& res) {
            res["data"] = nlohmann::json::object({});

            std::list<std::string> filenames;
            app_common::JsonUtils::field_should_be_array(req, "filenames");
            for (auto& item : req["filenames"]) {
                filenames.push_back(item.get<std::string>());
            }

            for (auto filename : filenames) {
                if (!_gpx_file_handler.gpx_file_using(filename)) {
                    continue;
                }
                throw std::runtime_error("不能删除正在使用的文件");
            }

            _gpx_file_handler.gpx_file_remove(filenames);
        }

        void Update(const nlohmann::json& req, nlohmann::json& res) {
            std::string old_name;
            std::string road_name;
            std::string start_name;
            std::string end_name;
            std::string patrol_mode;

            app_common::JsonUtils::valueFromJson(old_name, req, "old_name", true);
            app_common::JsonUtils::valueFromJson(road_name, req, "road_name", true);
            app_common::JsonUtils::valueFromJson(start_name, req, "start_name", true);
            app_common::JsonUtils::valueFromJson(end_name, req, "end_name", true);
            app_common::JsonUtils::valueFromJson(patrol_mode, req, "patrol_mode", true);

            res["data"] = nlohmann::json::object({});

            _gpx_file_handler.gpx_file_start_end(old_name, road_name, start_name, end_name);
        }

    protected:
        struct MapItem {
            std::string   filename;
            std::int32_t  size;
            std::uint32_t update_time;
        };

        app_common::GpxFileHandler _gpx_file_handler;
    };

} // namespace app_common
#endif
