#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>   // 仅需这一个头文件
#include "tools.h"

using json = nlohmann::json;

/*=================  ParamInfo  =================*/
struct ParamInfo
{
    std::string param_name;
    std::string param_type;
    std::string param_description;
    std::string param_dir;   // In / InOut / Top / RETURN
    std::string param_value;
    int         index = -1;

    ParamInfo() = default;
    explicit ParamInfo(const std::string& name) : param_name(name) {}

    void Print(const std::string& prefix = "") const
    {
        std::cout << prefix << "param_name:" << param_name << '\n';
        std::cout << prefix << "param_type:" << param_type << '\n';
        std::cout << prefix << "param_description:" << param_description << '\n';
        std::cout << prefix << "param_dir:" << param_dir << '\n';
        std::cout << prefix << "index:" << index << '\n';
        if (!param_value.empty())
            std::cout << prefix << "param_value:" << param_value << '\n';
    }

    /*---- 序列化 / 反序列化 ----*/
    json to_json() const
    {
        return json{
            {"param_name",        param_name},
            {"param_type",        param_type},
            {"param_description", param_description},
            {"param_dir",         param_dir},
            {"param_value",       param_value},
            {"index",             index}
        };
    }

    static ParamInfo from_json(const json& j)
    {
        ParamInfo p;
        p.param_name        = j.value("param_name",        "");
        p.param_type        = j.value("param_type",        "");
        p.param_description = j.value("param_description", "");
        p.param_dir         = j.value("param_dir",         "");
        p.param_value       = j.value("param_value",       "");
        p.index             = j.value("index",             -1);
        return p;
    }
};

/*=================  ParamSetting  =================*/
struct ParamSetting
{
    ParamInfo param;
    double min = 0.0;
    double max = 100.0;

    void Print(const std::string& prefix = "") const
    {
        param.Print(prefix);
        std::cout << prefix << "min:" << min << '\n';
        std::cout << prefix << "max:" << max << '\n';
    }

    json to_json() const
    {
        return json{
            {"param_name",        param.param_name},
            {"param_type",        param.param_type},
            {"param_description", param.param_description},
            {"param_value",       param.param_value},
            {"min",               min},
            {"max",               max}
        };
    }

    static ParamSetting from_json(const json& j)
    {
        ParamSetting ps;
        ps.param.param_name        = j.value("param_name",        "");
        ps.param.param_type        = j.value("param_type",        "");
        ps.param.param_description = j.value("param_description", "");
        ps.param.param_value       = j.value("param_value",       "");
        ps.min                     = j.value("min",               0.0);
        ps.max                     = j.value("max",               100.0);
        return ps;
    }
};

/*=================  ModuleConfig  =================*/
class ModuleConfig
{
public:
    std::map<std::string, std::vector<ParamSetting>> function_config;

    ModuleConfig() = default;

    std::vector<ParamSetting> GetCurrentParam() const
    {
        auto it = function_config.find("this");
        return (it == function_config.end()) ? std::vector<ParamSetting>{} : it->second;
    }

    int GetParam(const std::string& key,
                 const std::string& param_name,
                 std::string&       res) const
    {
        auto it = function_config.find(key);
        if (it == function_config.end()) return -1;
        for (const auto& p : it->second)
            if (p.param.param_name == param_name) { res = p.param.param_value; return 0; }
        return -1;
    }

    int UpdateParam(const std::string& key,
                    const std::string& param_name,
                    const std::string& param_value)
    {
        auto it = function_config.find(key);
        if (it == function_config.end()) return -1;
        for (auto& p : it->second)
            if (p.param.param_name == param_name) { p.param.param_value = param_value; return 0; }
        return -1;
    }

    void Print(const std::string& prefix = "") const
    {
        std::cout << prefix << "function_config:\n";
        for (const auto& [path, vec] : function_config)
        {
            std::cout << prefix << "\tpath:" << path << '\n';
            for (const auto& ps : vec) ps.Print(prefix + "\t\t");
        }
    }

    /*---------- 序列化 ----------*/
    json to_json() const
    {
        json j;
        for (const auto& [path, vec] : function_config)
        {
            json arr = json::array();
            for (const auto& ps : vec) arr.push_back(ps.to_json());
            j["function_config"][path] = arr;
        }
        return j;
    }

    void from_json(const json& j)
    {
        function_config.clear();
        if (!j.contains("function_config")) return;
        for (auto& [path, arr] : j["function_config"].items())
        {
            std::vector<ParamSetting> vec;
            for (const auto& item : arr) vec.push_back(ParamSetting::from_json(item));
            function_config[path] = std::move(vec);
        }
    }

    /*---------- 文件读写 ----------*/
    int SerializationToFile(const std::string& file_path) const
    {
        std::ofstream ofs(file_path);
        if (!ofs) { std::cerr << "PluginInfoMap SerializationToFile failed\n"; return -1; }
        ofs << to_json().dump(4);   // 4 空格缩进
        ofs.flush();
        return 0;
    }

    int DeserializationFromFile(const std::string& file_path)
    {
        std::ifstream ifs(file_path);
        if (!ifs) { std::cerr << "PluginInfoMap DeserializationFromFile open file failed " << file_path << '\n'; return -1; }
        try
        {
            json j;
            ifs >> j;
            from_json(j);
        }
        catch (const std::exception& e)
        {
            std::cerr << "PluginInfoMap parse error: " << e.what() << '\n';
            return -1;
        }
        return 0;
    }

    /* 兼容旧接口，直接转发 */
    int DeserializationProjectFromFile(const std::string& file_path) { return DeserializationFromFile(file_path); }
};