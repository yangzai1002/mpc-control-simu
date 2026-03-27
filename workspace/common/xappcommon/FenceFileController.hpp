#ifndef FENCE_FILE_CONTROLLER_HPP
#define FENCE_FILE_CONTROLLER_HPP

#include "GpxFileController.hpp"
#include "JsonUtils.hpp"
#include "custom_stack.h"
#include "log/log.h"

namespace app_common {

    class FenceFileController : public GpxFileController {
    public:
        explicit FenceFileController(const std::string& base_dir) : GpxFileController(base_dir, "default_fence.txt") {
        }

        void Create(const nlohmann::json& req, nlohmann::json& res) {
            std::string req_name;
            app_common::JsonUtils::valueFromJson(req_name, req, "name", true);
            if (req_name.empty()) {
                throw std::runtime_error("非法请求");
            }
            if (_gpx_file_handler.gpx_file_exist(req_name)) {
                throw std::runtime_error("文件 " + req_name + " 已存在");
            }
            _gpx_file_handler.gpx_file_new(req_name, "", "");
            _gpx_file_handler.using_gpx_file(req_name);
        }

        void Get(const nlohmann::json& req, nlohmann::json& res) {
            try {
                auto using_file = _gpx_file_handler.using_gpx_file();
                auto using_file_size = _gpx_file_handler.gpx_file_size(using_file);
                res["data"] = nlohmann::json::object({
                    {"name", using_file},
                    {"size", using_file_size},
                });
            } catch (const std::exception& e) {
                AERROR << e.what();
                throw std::runtime_error("默认围栏文件未设置");
            }
        }

        void Set(const nlohmann::json& req, nlohmann::json& res) {
            std::string req_name;
            app_common::JsonUtils::valueFromJson(req_name, req, "name", true);
            if (req_name.empty()) {
                throw std::runtime_error("非法请求");
            }
            if (!_gpx_file_handler.gpx_file_exist(req_name)) {
                throw std::runtime_error("文件 " + req_name + " 不存在");
            }

            _gpx_file_handler.using_gpx_file(req_name);
        }
    }; // namespace app_common
} // namespace app_common
#endif
