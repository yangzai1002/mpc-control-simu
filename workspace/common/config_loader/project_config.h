#include <dirent.h> // 用于目录遍历
#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "log/log.h"

class ProjectYamlConfig {
private:
    std::map<std::string, YAML::Node> key_data;

private:
    std::vector<std::string> findFilesWithExtension(const std::string& directoryPath, const std::string& extension) {
        std::vector<std::string> result;
        std::regex               pattern(".*\\." + extension + "$"); // 正则表达式匹配指定扩展名的文件

        DIR* dir = opendir(directoryPath.c_str()); // 打开目录
        if (dir == nullptr) {
            AERROR << "Failed to open directory: " << directoryPath;
            return result; // 返回空列表
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) { // 遍历目录
            std::string filename(entry->d_name);
            if (std::regex_match(filename, pattern)) {            // 检查文件名是否匹配正则表达式
                result.push_back(directoryPath + "/" + filename); // 添加全路径到结果列表
            }
        }
        closedir(dir); // 关闭目录

        return result;
    }

    void printNode(const YAML::Node& node, std::string path = "") {
        switch (node.Type()) {
            case YAML::NodeType::Null:
                std::cout << path << "null" << std::endl;
                break;
            case YAML::NodeType::Scalar:
                key_data[path] = node;
                std::cout << path << "'" << node.as<std::string>() << "'" << std::endl;
                break;
            case YAML::NodeType::Sequence:
                key_data[path] = node;
                for (std::size_t i = 0; i < node.size(); ++i) {
                    printNode(node[i], path + "[" + std::to_string(i) + "]");
                }
                break;
            case YAML::NodeType::Map:
                for (const auto& pair : node) {
                    printNode(pair.second, path + pair.first.as<std::string>() + "/");
                }
                break;
            default:
                break;
        }
    }

    void Print() {
        for (auto& p : key_data) {
            std::cout << "p:" << p.first << std::endl;
        }
    }

    // 辅助函数，用于通过多级键访问 YAML 节点
    bool getNodeByMultiKey(std::string key, YAML::Node& res_node) {
        if (key == "") {
            return false;
        }
        if (key.back() != '/') {
            key += '/';
        }
        auto it = key_data.find(key);
        if (it == key_data.end()) {
            return false;
        }

        res_node = it->second;
        return true;
    }

public:
    void CheckOtherConfig() {
        std::string car_name, vin_file, vehicle_config_path;
        if (getValue("vehicle_config_path", vehicle_config_path) && !vehicle_config_path.empty()) {
            if (getValue("current_vehicle", car_name) && !car_name.empty()) {
                AINFO << "current vehicle name:" << car_name;
                AINFO << "vehicle config path:" << vehicle_config_path;
                auto path_vec = findFilesWithExtension(vehicle_config_path + "/" + car_name, "proj_cfg");
                if (path_vec.size() == 0) {
                    AERROR << "cannot load " << vehicle_config_path + "/" + car_name;
                }
                for (auto& p : path_vec) {
                    AINFO << "get config:" << p;
                    loadFile(p);
                }
            } else if (getValue("vin_file", vin_file) && !vin_file.empty()) {
                AINFO << "vin file:" << vin_file;
                auto path_vec = findFilesWithExtension(vehicle_config_path, "proj_cfg");
                if (path_vec.size() == 0) {
                    AERROR << "cannot load " << vehicle_config_path;
                }
                for (auto& p : path_vec) {
                    AINFO << "get config:" << p;
                    loadFile(p);
                }
            } else {
                AERROR << "neither current_vehicle nor vin_file is set";
            }
        }
    }

    // 加载 YAML 文件内容并合并到 mergedConfig
    void loadFile(const std::string& filePath) {
        std::ifstream fin(filePath);
        printNode(YAML::Load(fin));
        // Print();
    }

    // 模板函数，用于获取值并存储到 data 中
    template <typename T> bool getValue(const std::string& key, T& data) {
        YAML::Node node;
        if (getNodeByMultiKey(key, node)) {
            try {
                data = node.as<T>();
                return true;
            } catch (const YAML::TypedBadConversion<T>&) {
                // 类型转换失败，返回 false
                return false;
            }
        }
        return false; // 键不存在或类型不匹配，返回 false
    }

    // 模板函数，用于获取值并存储到 data 中
    template <typename T> T getValue(const std::string& key) {
        YAML::Node node;
        getNodeByMultiKey(key, node);
        return node.as<T>();
    }

    // 模板函数，用于获取数组类型的值
    template <typename T> bool getArray(const std::string& key, std::vector<T>& array) {
        YAML::Node node;
        if (getNodeByMultiKey(key, node) && node.IsSequence()) {
            array.clear();
            for (const auto& elem : node) {
                try {
                    array.push_back(elem.as<T>());
                } catch (const YAML::TypedBadConversion<T>&) {
                    // 如果某个元素类型不匹配，返回 false
                    return false;
                }
            }
            return true;
        }
        return false; // 键不存在或不是数组，返回 false
    }
};

// int main() {
//     ProjectYamlConfig config;
//     config.loadFile("temp/select.proj_cfg");
//     config.CheckOtherConfig();
//     // 可以继续加载更多文件...

//     std::string name;
//     double body_base_height;
//     std::vector<std::string> lidar_name;
//     std::vector<std::string> camera_name;
//     std::vector<double> lidar_t;
//     std::vector<double> D;

//     // 查询合并后的配置，支持多级键
//     if (config.getValue("vehicle_interface/body_base_height", body_base_height)) {
//         std::cout << "body_base_height: " << body_base_height << std::endl;
//     }
//     if (config.getValue("vehicle_interface/car_name", name)) {
//         std::cout << "name: " << name << std::endl;
//     }
//     if (config.getArray("lidar_names", lidar_name)) {
//         std::cout << "lidar_name: ";
//         for (const auto& lidar : lidar_name) {
//             std::cout << lidar << " ";
//         }
//         std::cout << std::endl;
//     }

//     if (config.getArray("camera_names", camera_name)) {
//         std::cout << "camera_names: ";
//         for (const auto& camera : camera_name) {
//             std::cout << camera << " ";
//         }
//         std::cout << std::endl;
//     }

//     if (config.getArray("front_m1p/lidar_t", lidar_t)) {
//         std::cout << "front_m1p t: ";
//         for (const auto& t : lidar_t) {
//             std::cout << t << " ";
//         }
//         std::cout << std::endl;
//     }

//     if (config.getArray("front_h125/D", D)) {
//         std::cout << "D: ";
//         for (const auto& t : D) {
//             std::cout << t << " ";
//         }
//         std::cout << std::endl;
//     }

//     return 0;
// }