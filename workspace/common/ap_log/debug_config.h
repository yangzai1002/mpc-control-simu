#pragma once

#include "nlohmann/json.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <regex>
#include <string>

#define NLOHMANN_JSON_SERIALIZATION_FUNCTION                         \
    int SerializationToFile(const std::string& file_path) { \
        std::ofstream ofs;                                  \
        ofs.open(file_path);                                \
        if (ofs.is_open() == false) {                       \
            return -1;                                      \
        }                                                   \
                                                            \
        nlohmann::json root;                                \
        SerializationToJson(root);                          \
        std::string json_file = root.dump(4);               \
        ofs << json_file;                                   \
        ofs.close();                                        \
        return 0;                                           \
    }

#define NLOHMANN_JSON_DESERIALIZATION_FUNCTION                                                    \
    int DeserializationFromFile(const std::string& file_path) {                          \
        std::ifstream file(file_path);                                                   \
        if (!file.is_open()) {                                                           \
            return -1;                                                                   \
        }                                                                                \
        nlohmann::json root;                                                             \
        try {                                                                            \
            file >> root;                                                                \
        } catch (...) {                                                                  \
            file.close();                                                                \
            return -1;                                                                   \
        }                                                                                \
        int result = DeserializationFromJson(root);                                      \
        file.close();                                                                    \
        return result;                                                                   \
    }

struct NodeDebugInfo {
    std::string node_name;
    std::string log_prefix;
    int debug_mode;  // 0:debug模式  1:release模式
    int log_level;   // 0:debug  1:warning 2:error

    NodeDebugInfo() : debug_mode(0), node_name(""), log_level(0) {}

    int SerializationToJson(nlohmann::json& value) const {
        value["debug_mode"] = debug_mode;
        value["log_prefix"] = log_prefix;
        value["node_name"] = node_name;
        value["log_level"] = log_level;
        return 0;
    }

    int DeserializationFromJson(const nlohmann::json& value) {
        try {
            log_prefix = value.at("log_prefix").get<std::string>();
            debug_mode = value.at("debug_mode").get<int>();
            node_name = value.at("node_name").get<std::string>();
            log_level = value.at("log_level").get<int>();
            return 0;
        } catch (...) {
            return -1;
        }
    }

    void Print(){
    	std::cout << "node_name:"<<node_name<<std::endl;
    	std::cout << "debug_mode:"<<debug_mode<<std::endl;
    	std::cout << "log_level:"<<log_level<<std::endl;
    }
};

struct NodeDebugInfoVec {
    std::string describe;
    std::string log_save_path;
    std::vector<NodeDebugInfo> debug_info;

    NodeDebugInfoVec() : describe("debug_mode:(0:debug模式  1:release模式)     log_level:(0:debug  1:warning 2:error) ") {
        log_save_path = "/opt/usr/log/";
    }


    void Print(){
    	for(auto &p:debug_info){
    		p.Print();
    	}
    }
    int SerializationToJson(nlohmann::json& value) const {
        value["describe"] = describe;
        value["log_save_path"] = log_save_path;
        nlohmann::json debug_array = nlohmann::json::array();
        for (const auto& info : debug_info) {
            nlohmann::json item;
            info.SerializationToJson(item);
            debug_array.push_back(item);
        }
        value["debug_info"] = debug_array;
        return 0;
    }

    int DeserializationFromJson(const nlohmann::json& value) {
        try {
            log_save_path = value.at("log_save_path").get<std::string>();
            describe = value.at("describe").get<std::string>();
            const auto& debug_array = value.at("debug_info");
            debug_info.clear();
            for (const auto& item : debug_array) {
                NodeDebugInfo info;
                if (info.DeserializationFromJson(item) == 0) {
                    debug_info.push_back(info);
                }
            }
            return 0;
        } catch (...) {
            return -1;
        }
    }

    std::string GetLogPrefix(const std::string& node_name)const{
        for (const auto& p : debug_info) {
            if (p.node_name == node_name) {
                return p.log_prefix;
            }
        }
        return node_name;
    }
    int GetLogLevel(const std::string& node_name) const {
        for (const auto& p : debug_info) {
            if (p.node_name == node_name) {
                return p.log_level;
            }
        }
        return 2;  // default is error
    }

    int GetDebugLevel(const std::string& node_name) const {
        for (const auto& p : debug_info) {
            if (p.node_name == node_name) {
                return p.debug_mode;
            }
        }
        return 1;  // default is release
    }

    NLOHMANN_JSON_SERIALIZATION_FUNCTION
    NLOHMANN_JSON_DESERIALIZATION_FUNCTION
};
