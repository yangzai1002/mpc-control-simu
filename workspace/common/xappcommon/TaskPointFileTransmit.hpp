#ifndef TASK_POINT_FILE_TRANSMIT_HPP
#define TASK_POINT_FILE_TRANSMIT_HPP

#include <glog/logging.h>
#include <openssl/md5.h>

#include <filesystem>
#include <limits>

#include "JsonUtils.hpp"
#include "custom_stack.h"

class TaskPointFileTransmit {
public:
    TaskPointFileTransmit(const std::string& filePath, const std::string& suffixCheck)
        : cur_file_path_(filePath), suffixCheck_(suffixCheck) {
    }

    TaskPointFileTransmit() = default;

    ~TaskPointFileTransmit() = default;

    std::string MD5(const std::string& src) {
        MD5_CTX       ctx;
        std::string   md5_str;
        unsigned char md[16] = {0};
        char          tmp[33] = {0};
        MD5_Init(&ctx);
        MD5_Update(&ctx, src.c_str(), src.size());
        MD5_Final(md, &ctx);
        for (size_t i = 0; i < 16; ++i) {
            memset(tmp, 0x00, sizeof(tmp));
            sprintf(tmp, "%02x", md[i]);
            md5_str += tmp;
        }
        return md5_str;
    }

    std::string GetFileName(std::string name, bool with_suffix = true) {
        std::string filename = name;
        if (with_suffix) {
            filename += suffixCheck_;
        }
        std::filesystem::path filepath = cur_file_path_;
        filepath.append(filename);
        return filepath.string();
    }

    bool GetAbsFilePath(std::string file_name, std::string& absFilePath) {
        char abs_path_buf[256];
        if (realpath(file_name.c_str(), abs_path_buf)) {
            absFilePath = abs_path_buf;
            return true;
        } else {
            return false;
        }
    }

    void UploadCb(const nlohmann::json& req, nlohmann::json& res) {
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

        if (current_slice < slice_total) {
            // 等待剩下的包
            return;
        }

        if (buffer.size() != size) {
            LOG(WARNING) << "file size not equal: req=" << size << ", buffer=" << buffer.size();
            throw std::runtime_error("文件大小不匹配");
        }

        if (md5 != MD5(buffer)) {
            throw std::runtime_error("文件MD5校验失败");
        }

        if (filename == "") {
            LOG(WARNING) << "filename is empty.";
            throw std::runtime_error("名称不能为空");
        }

        std::filesystem::path filepath = cur_file_path_;
        filepath.append(filename);

        std::ofstream os(filepath);
        if (!os.is_open()) {
            throw std::runtime_error("Template UploadCb 文件写入失败");
        }
        os << buffer;
        os.close();
        current_slice = 0;
        buffer.clear();
    }

    void DownloadCb(const nlohmann::json& req, nlohmann::json& res) {
        std::string  filename;
        std::int16_t slice_index;
        std::int32_t slice_size;

        app_common::JsonUtils::valueFromJson(filename, req, "filename", true);
        app_common::JsonUtils::valueFromJson(slice_index, req, "slice_index", true);
        app_common::JsonUtils::valueFromJson(slice_size, req, "slice_size", true);

        LOG(INFO) << "service: download: " << filename << ", " << slice_index;

        std::filesystem::path path = cur_file_path_;
        path.append(filename);
        if (!std::filesystem::exists(path) || filename == "") {
            LOG(WARNING) << "file(" << filename << ") not exists.";
            throw std::runtime_error("文件不存在");
        }
        uint32_t req_slice_size = slice_size;
        if (slice_size == 0) {
            req_slice_size = std::numeric_limits<std::uint32_t>::max();
        }
        static std::string buffer;
        static std::string md5;
        static std::string buferr_filename;
        if (slice_index == 0) {
            std::ifstream is(path);
            if (!is.is_open()) {
                throw std::runtime_error("文件打开失败");
            }
            std::ostringstream sstr;
            sstr << is.rdbuf();
            is.close();
            buffer = sstr.str();
            md5 = MD5(buffer);
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

    void ListCb(const nlohmann::json& req, nlohmann::json& res) {
        res["data"] = nlohmann::json::object({{"list", nlohmann::json::array()}});
        for (std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(cur_file_path_)) {
            const std::filesystem::path& path = entry.path();
            if (!std::filesystem::is_regular_file(path)) {
                continue;
            }
            std::string ext = std::filesystem::extension(path);
            if (ext != suffixCheck_) {
                continue;
            }
            std::string name = path.filename().string();
            res["data"]["list"].push_back(
                nlohmann::json({{"filename", name},
                                {"size", std::filesystem::file_size(path)},
                                {"update_time", std::filesystem::last_write_time(path).time_since_epoch().count()}}));
        }
    }

    void DeleteCb(const nlohmann::json& req, nlohmann::json& res) {
        std::list<std::string> filenames;
        for (auto& item : req["filenames"]) {
            filenames.push_back(item.get<std::string>());
        }

        res["data"] = nlohmann::json::object({});

        for (auto filename : filenames) {
            LOG(INFO) << "delete file: " << filename.c_str();
            if (GetFileName(cur_file_) != GetFileName(filename, false)) {
                continue;
            }
            throw std::runtime_error("不能删除当前使用的文件");
        }

        for (auto filename : filenames) {
            std::string path = GetFileName(filename, false);
            LOG(INFO) << "delete file: " << path.c_str();
            std::filesystem::remove(path);
        }
    }

    std::string GetFilePath() {
        return cur_file_path_;
    }

    std::string GetSuffixCheck() {
        return suffixCheck_;
    }

    std::string GetCurFile() {
        return cur_file_;
    }

    void SetCurFile(std::string curfile) {
        cur_file_ = curfile;
    }

private:
    const std::string cur_file_path_;
    const std::string suffixCheck_; //后缀名检查

    std::string cur_file_;
};
#endif