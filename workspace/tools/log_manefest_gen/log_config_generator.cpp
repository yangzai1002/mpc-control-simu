#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <map>
#include <algorithm>

namespace fs = std::filesystem;

// JSON 解析和生成类（简化版实现）
class JsonParser {
public:
    struct Value {
        enum Type { Object, Array, String, Number, Bool, Null };
        Type type;
        std::string str_value;
        double num_value;
        bool bool_value;
        std::map<std::string, Value> obj_value;
        std::vector<Value> arr_value;

        Value() : type(Null), num_value(0), bool_value(false) {}
        Value(const std::string& s) : type(String), str_value(s), num_value(0), bool_value(false) {}
        Value(const char* s) : type(String), str_value(s), num_value(0), bool_value(false) {}
        Value(double n) : type(Number), num_value(n), bool_value(false) {}
        Value(int n) : type(Number), num_value(n), bool_value(false) {}
        Value(bool b) : type(Bool), str_value(""), num_value(0), bool_value(b) {}

        std::string toString(int indent = 0) const {
            std::string indent_str(indent, ' ');
            switch (type) {
                case String:
                    return "\"" + escapeString(str_value) + "\"";
                case Number:
                    return num_value == static_cast<int>(num_value) ?
                           std::to_string(static_cast<int>(num_value)) :
                           std::to_string(num_value);
                case Bool:
                    return bool_value ? "true" : "false";
                case Null:
                    return "null";
                case Object: {
                    std::string s = "{\n";
                    bool first = true;
                    for (const auto& [key, val] : obj_value) {
                        if (!first) s += ",\n";
                        first = false;
                        s += indent_str + "  \"" + key + "\": " + val.toString(indent + 2);
                    }
                    s += "\n" + indent_str + "}";
                    return s;
                }
                case Array: {
                    std::string s = "[";
                    bool first = true;
                    for (const auto& val : arr_value) {
                        if (!first) s += ", ";
                        first = false;
                        s += val.toString(indent);
                    }
                    s += "]";
                    return s;
                }
            }
            return "";
        }

    private:
        static std::string escapeString(const std::string& s) {
            std::string result;
            for (char c : s) {
                if (c == '"') result += "\\\"";
                else if (c == '\\') result += "\\\\";
                else if (c == '\n') result += "\\n";
                else if (c == '\r') result += "\\r";
                else if (c == '\t') result += "\\t";
                else result += c;
            }
            return result;
        }
    };

    static Value parse(const std::string& json) {
        size_t pos = 0;
        return parseValue(json, pos);
    }

    static std::string generate(const Value& v) {
        return v.toString(0);
    }

private:
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' ||
                                     json[pos] == '\n' || json[pos] == '\r')) {
            pos++;
        }
    }

    static Value parseValue(const std::string& json, size_t& pos) {
        skipWhitespace(json, pos);
        if (pos >= json.size()) return Value();

        if (json[pos] == '{') return parseObject(json, pos);
        if (json[pos] == '[') return parseArray(json, pos);
        if (json[pos] == '"') return parseString(json, pos);
        if (json[pos] == 't' || json[pos] == 'f') return parseBool(json, pos);
        if (json[pos] == 'n') return parseNull(json, pos);
        return parseNumber(json, pos);
    }

    static Value parseObject(const std::string& json, size_t& pos) {
        Value v;
        v.type = Value::Object;
        pos++; // skip '{'
        skipWhitespace(json, pos);

        while (pos < json.size() && json[pos] != '}') {
            skipWhitespace(json, pos);
            if (json[pos] == '}') break;
            if (json[pos] == ',') pos++;

            skipWhitespace(json, pos);
            std::string key = parseString(json, pos).str_value;

            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') return v;
            pos++; // skip ':'

            Value val = parseValue(json, pos);
            v.obj_value[key] = val;
        }
        pos++; // skip '}'
        return v;
    }

    static Value parseArray(const std::string& json, size_t& pos) {
        Value v;
        v.type = Value::Array;
        pos++; // skip '['
        skipWhitespace(json, pos);

        while (pos < json.size() && json[pos] != ']') {
            skipWhitespace(json, pos);
            if (json[pos] == ']') break;
            if (json[pos] == ',') pos++;

            skipWhitespace(json, pos);
            v.arr_value.push_back(parseValue(json, pos));
        }
        pos++; // skip ']'
        return v;
    }

    static Value parseString(const std::string& json, size_t& pos) {
        Value v;
        v.type = Value::String;
        pos++; // skip '"'

        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos++;
                char c = json[pos];
                switch (c) {
                    case '"': v.str_value += '"'; break;
                    case '\\': v.str_value += '\\'; break;
                    case 'n': v.str_value += '\n'; break;
                    case 'r': v.str_value += '\r'; break;
                    case 't': v.str_value += '\t'; break;
                    default: v.str_value += c;
                }
            } else {
                v.str_value += json[pos];
            }
            pos++;
        }
        pos++; // skip closing '"'
        return v;
    }

    static Value parseNumber(const std::string& json, size_t& pos) {
        Value v;
        v.type = Value::Number;
        std::string num_str;

        while (pos < json.size() && (isdigit(json[pos]) || json[pos] == '.' ||
                                      json[pos] == '-' || json[pos] == '+' ||
                                      json[pos] == 'e' || json[pos] == 'E')) {
            num_str += json[pos++];
        }
        v.num_value = std::stod(num_str);
        return v;
    }

    static Value parseBool(const std::string& json, size_t& pos) {
        Value v;
        v.type = Value::Bool;
        if (json.substr(pos, 4) == "true") {
            v.bool_value = true;
            pos += 4;
        } else if (json.substr(pos, 5) == "false") {
            v.bool_value = false;
            pos += 5;
        }
        return v;
    }

    static Value parseNull(const std::string& json, size_t& pos) {
        Value v;
        v.type = Value::Null;
        if (json.substr(pos, 4) == "null") {
            pos += 4;
        }
        return v;
    }
};

// 日志模块配置结构
struct LogModule {
    int debug_mode;
    int log_level;
    std::string log_prefix;
    int logTraceSingleFileSize;
    int logTraceFileNum;
    std::string node_name;
};

// 从 log.conf 读取配置
std::vector<LogModule> parseLogConfig(const std::string& config_path) {
    std::vector<LogModule> modules;

    std::ifstream file(config_path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << config_path << std::endl;
        return modules;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    JsonParser::Value root = JsonParser::parse(content);

    // 获取 debug_info 数组
    if (root.type == JsonParser::Value::Object &&
        root.obj_value.count("debug_info")) {
        const auto& debug_info = root.obj_value.at("debug_info");

        if (debug_info.type == JsonParser::Value::Array) {
            for (const auto& item : debug_info.arr_value) {
                if (item.type == JsonParser::Value::Object) {
                    LogModule module;
                    const auto& obj = item.obj_value;

                    if (obj.count("debug_mode")) {
                        module.debug_mode = static_cast<int>(obj.at("debug_mode").num_value);
                    }
                    if (obj.count("log_level")) {
                        module.log_level = static_cast<int>(obj.at("log_level").num_value);
                    }
                    if (obj.count("log_prefix")) {
                        module.log_prefix = obj.at("log_prefix").str_value;
                    }
                    if (obj.count("logTraceSingleFileSize")) {
                        module.logTraceSingleFileSize = static_cast<int>(
                            obj.at("logTraceSingleFileSize").num_value);
                    }
                    if (obj.count("logTraceFileNum")) {
                        module.logTraceFileNum = static_cast<int>(
                            obj.at("logTraceFileNum").num_value);
                    }
                    if (obj.count("node_name")) {
                        module.node_name = obj.at("node_name").str_value;
                    }

                    modules.push_back(module);
                }
            }
        }
    }

    return modules;
}

// 转换日志级别
std::string convertLogLevel(int level) {
    switch (level) {
        case 0: return "info";
        case 1: return "warn";
        case 2: return "error";
        default: return "info";
    }
}

// 生成 LOG_MANIFEST.json
JsonParser::Value generateLogManifest(const LogModule& module,
                                        const std::string& log_save_path) {
    JsonParser::Value manifest;
    manifest.type = JsonParser::Value::Object;

    // 设置日志行为开关
    JsonParser::Value loggingBehavior;
    loggingBehavior.type = JsonParser::Value::Bool;
    loggingBehavior.bool_value = true;
    manifest.obj_value["loggingBehavior"] = loggingBehavior;

    // 设置进程ID (对应 log_prefix)
    JsonParser::Value processId;
    processId.type = JsonParser::Value::String;
    processId.str_value = module.log_prefix;
    manifest.obj_value["logTraceProcessId"] = processId;

    // 设置进程描述
    JsonParser::Value processDesc;
    processDesc.type = JsonParser::Value::String;
    processDesc.str_value = "DM_ARA_LOG";
    manifest.obj_value["logTraceProcessDesc"] = processDesc;

    // 设置日志模式
    JsonParser::Value logModes;
    logModes.type = JsonParser::Value::Array;
    JsonParser::Value fileMode;
    fileMode.type = JsonParser::Value::String;
    fileMode.str_value = "file";
    logModes.arr_value.push_back(fileMode);
    manifest.obj_value["logTraceLogModes"] = logModes;

    // 设置默认日志级别
    JsonParser::Value logLevel;
    logLevel.type = JsonParser::Value::String;
    logLevel.str_value = convertLogLevel(module.log_level);
    manifest.obj_value["logTraceDefaultLogLevel"] = logLevel;

    // 设置单个文件大小
    JsonParser::Value singleFileSize;
    singleFileSize.type = JsonParser::Value::Number;
    singleFileSize.num_value = module.logTraceSingleFileSize;
    manifest.obj_value["logTraceSingleFileSize"] = singleFileSize;

    // 设置文件数量
    JsonParser::Value fileNum;
    fileNum.type = JsonParser::Value::Number;
    fileNum.num_value = module.logTraceFileNum;
    manifest.obj_value["logTraceFileNum"] = fileNum;

    // 设置日志文件路径
    JsonParser::Value filePath;
    filePath.type = JsonParser::Value::String;
    filePath.str_value = log_save_path;
    manifest.obj_value["logTraceFilePath"] = filePath;

    return manifest;
}

// 从模板文件读取 JSON
JsonParser::Value readTemplateFile(const std::string& template_path) {
    std::ifstream file(template_path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open template file: " << template_path << std::endl;
        return JsonParser::Value();
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    return JsonParser::parse(content);
}

// 处理每个模块
void processModules(const std::vector<LogModule>& modules,
                   const std::string& path2,
                   const std::string& log_save_path) {
    int success_count = 0;
    int fail_count = 0;

    for (const auto& module : modules) {
        if (module.node_name.empty()) {
            std::cerr << "Warning: module with empty node_name, skipping" << std::endl;
            continue;
        }

        // 构建目标目录路径: path2/node_name/etc
        std::string target_dir = path2 + "/" + module.node_name + "/etc";

        // 检查目录是否存在
        if (!fs::exists(target_dir)) {
            std::cerr << "Warning: Directory does not exist: " << target_dir << std::endl;
            fail_count++;
            continue;
        }

        // 生成 LOG_MANIFEST.json 内容
        JsonParser::Value manifest = generateLogManifest(module, log_save_path);
        std::string manifest_content = JsonParser::generate(manifest);

        // 写入目标文件
        std::string target_file = target_dir + "/LOG_MANIFEST.json";
        std::ofstream out_file(target_file);
        if (out_file.is_open()) {
            out_file << manifest_content << std::endl;
            out_file.close();
            std::cout << "Success: Generated " << target_file << std::endl;
            success_count++;
        } else {
            std::cerr << "Error: Cannot write to file: " << target_file << std::endl;
            fail_count++;
        }
    }

    std::cout << "\nSummary:" << std::endl;
    std::cout << "  Success: " << success_count << std::endl;
    std::cout << "  Failed:  " << fail_count << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <path1> <path2>" << std::endl;
        std::cout << "  path1: Path containing log.conf" << std::endl;
        std::cout << "  path2: Path containing node directories" << std::endl;
        return 1;
    }

    std::string path1 = argv[1];
    std::string path2 = argv[2];

    // 构建 log.conf 路径
    std::string log_config_path = path1 + "/log.conf";

    std::cout << "Reading config from: " << log_config_path << std::endl;

    // 读取并解析 log.conf
    std::vector<LogModule> modules = parseLogConfig(log_config_path);

    if (modules.empty()) {
        std::cerr << "Error: No modules found in config file" << std::endl;
        return 1;
    }

    std::cout << "Found " << modules.size() << " modules" << std::endl;

    // 从 log.conf 中读取 log_save_path
    std::ifstream file(log_config_path);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    JsonParser::Value root = JsonParser::parse(content);
    std::string log_save_path = "/opt/usr/zxz/log/";

    if (root.type == JsonParser::Value::Object &&
        root.obj_value.count("log_save_path")) {
        log_save_path = root.obj_value.at("log_save_path").str_value;
    }

    std::cout << "Log save path: " << log_save_path << std::endl;

    // 处理模块
    processModules(modules, path2, log_save_path);

    return 0;
}
