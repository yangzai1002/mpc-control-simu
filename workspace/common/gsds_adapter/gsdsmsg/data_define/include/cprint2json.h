#ifndef CPRINT2JSON_H
#define CPRINT2JSON_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 用于跟踪当前嵌套层次的上下文信息
struct Context {
    std::string key;
    std::string type;
    int cur_indent=0;
    bool is_array=false;
    bool is_map=false;
    nlohmann::json this_json;
    int prant_json_index;
    int cur_index=0;
};

class CPrint2Json
{
public:
    CPrint2Json() {}
    std::string print2json(const std::string & input)
    {
        m_jsonvec.clear();
        std::istringstream iss(input);
        std::string line;
        // 根据每一行生成json对象
        while (std::getline(iss, line)) {
            line.erase(0, line.find_first_not_of('\n')); // Remove leading newlines
            if (!line.empty()) {
                json j = json::object();
                parse2json(line, j);
            }
        }
        // 汇总json对象根据所在层位置倒排，如何在逐一汇总，得到最外层json
        build_json();
        if(m_jsonvec.size()>0)
            return m_jsonvec[0].this_json.dump(4);
        return "";
    }

private:
    // 计算行首的空格数，以确定缩进级别
    int calculate_indent(const std::string& line)
    {
        int indent = 0;
        for (char ch : line) {
            if (ch == ' ') {
                ++indent;
            } else {
                break;
            }
        }
        return indent;
    }
    // 解析字段名称、类型和值
    std::tuple<std::string, std::string, std::string> parse_field(const std::string& line)
    {
        std::regex field_regex(R"((\w+)(<([^>]+)>)?:\s*(.*))");
        std::regex field_regex2(R"((\w+)(?:\[\d+\])?<([^>]+)>:(\d+))");
        std::regex field_regex3(R"((\w+)(?:\[\d+\])?<([^>]+)>:\s*(.*))");
        std::smatch match;
        if (std::regex_search(line, match, field_regex)) {
            return {match[1], match[3], match[4]};
        }
        else if (std::regex_search(line, match, field_regex2)) {
            return {match[1], match[2], match[3]};
        }
        else if (std::regex_search(line, match, field_regex3)) {
            return {match[1], match[2], match[3]};
        }
        return {"", "", ""};
    }
    double isDoubleInRange(const char* value) {
        char* endptr;
        double num = std::strtod(value, &endptr);

        if (endptr != value && *endptr == '\0') {
//            std::cout << "Converted value: " << num << std::endl;
        } else {
//            std::cout << "Conversion failed or there are non-numeric characters in the string." << std::endl;
            num = 0;
        }

        return num;
    }
    json parse_value(const std::string& type, const std::string& value_line)
    {
        std::istringstream iss(value_line);
        std::string value;
        iss >> value;
        if (type == "int16" || type == "int32") {
            return std::stoi(value);
        }
        else if (type == "int8")
        {
            try {
                int tmp_int = std::stoi(value);
                if (tmp_int <= std::numeric_limits<uint8_t>::max()) {
                    return static_cast<int8_t>(tmp_int);
                } else {
                    std::cerr << "Value out of uint8_t range." << std::endl;
                    return -1;
                }
            } catch (const std::out_of_range&) {
                std::cerr << "Value out of range for unsigned long." << std::endl;
                return -1;
            } catch (const std::invalid_argument&) {
//                std::cerr << "Invalid argument for decimal conversion." << std::endl;
                return (int8_t)value.c_str()[0];
            }
        }
        else if (type == "uint8")
        {
            try {
                int tmp_int = std::stoul(value);
                if (tmp_int <= std::numeric_limits<uint8_t>::max()) {
                    return static_cast<int8_t>(tmp_int);
                } else {
                    std::cerr << "Value out of uint8_t range." << std::endl;
                    return -1;
                }
            } catch (const std::out_of_range&) {
                std::cerr << "Value out of range for unsigned long." << std::endl;
                return -1;
            } catch (const std::invalid_argument&) {
//                std::cerr << "Invalid argument for decimal conversion." << std::endl;
                return (uint8_t)value.c_str()[0];
            }

        }
        else if ( type == "uint16" || type == "uint32") {
            return std::stoul(value);
        }
        else if (type == "int64") {
            return std::stoll(value);
        }
        else if (type == "uint64") {
            return std::stoull(value);
        }
        else if (type == "bool") {
            return (value == "true" || value == "false") ? value == "true" : std::stoi(value);
        }
        else if (type == "double" || type == "float") {
            return isDoubleInRange(value.c_str());
        }
        else {
            return value;  // 其他类型或自定义类型，直接使用字符串
        }
    }
    void parse2json(const std::string& line, json& j)
    {
        int indent = calculate_indent(line);

        auto field_data = parse_field(line);
        // 获取 tuple 中的元素
        std::string field_name = std::get<0>(field_data);
        std::string type = std::get<1>(field_data);
        std::string value = std::get<2>(field_data);

        if(indent == 0)
        {
            j["type"] = "obj";
            Context cont;
            cont.key = field_name;
            cont.this_json = j;
            cont.cur_indent = indent;
            cont.cur_index = m_jsonvec.size();
            m_jsonvec.push_back(cont);
            m_pjsonindex[0]=0;
            return;
        }
        if (type == "std::set" ||type == "T[N]" ||type == "std::deque"||type == "std::list"||type == "std::vector" )
        {
            j["type"] = "obj";
            j["value"] = json::array();
            Context cont;
            cont.key = field_name;
            cont.type = type;
            cont.is_array = true;
            cont.this_json = j;
            cont.cur_indent = indent;
            cont.prant_json_index = m_pjsonindex[indent-1];
            cont.cur_index = m_jsonvec.size();
            m_jsonvec.push_back(cont);
            m_pjsonindex[indent]=m_jsonvec.size()-1;
            return;
        }
        else if (type == "int8"
                 || type == "int16"
                 || type == "int32"
                 || type == "int64"
                 || type == "uint8"
                 || type == "uint16"
                 || type == "uint32"
                 || type == "uint64"
                 || type == "float"
                 || type == "double"
                 || type == "bool"

                 ) {
            // 读取下一行作为值

            j["type"] = type;
            j["value"]  = parse_value(type, value);
            Context cont;
            cont.key = field_name;
            cont.type = type;
            cont.this_json = j;
            cont.cur_indent = indent;
            cont.prant_json_index = m_pjsonindex[indent-1];
            cont.cur_index = m_jsonvec.size();
            m_jsonvec.push_back(cont);
            m_pjsonindex[indent]=m_jsonvec.size()-1;
            return;
        }
        else if (type == "std::string") {
            j["type"] = type;
            j["value"]  = value;
            Context cont;
            cont.key = field_name;
            cont.type = type;
            cont.this_json = j;
            cont.cur_indent = indent;
            cont.prant_json_index = m_pjsonindex[indent-1];
            cont.cur_index = m_jsonvec.size();
            m_jsonvec.push_back(cont);
            m_pjsonindex[indent]=m_jsonvec.size()-1;
        } else {
            // 对于未知类型，可以在这里添加额外的处理
            Context cont;
            if(type == "")
            {
                j["type"] = "obj";
                cont.type = "obj";
            }
            else
            {
                j["type"] = type;
                j["value"]  = "no support show";
                cont.type = type;
                cont.is_map = true;
            }
            cont.key = field_name;
            cont.is_array = false;
            cont.this_json = j;
            cont.cur_indent = indent;
            cont.prant_json_index = m_pjsonindex[indent-1];
            cont.cur_index = m_jsonvec.size();
            m_jsonvec.push_back(cont);
            m_pjsonindex[indent]=m_jsonvec.size()-1;
        }
    }

    void build_json()
    {

        // 按 cur_indent 排序，确保层级顺序
        std::vector<Context> m_jsonvec_sort = m_jsonvec;
        std::sort(m_jsonvec_sort.begin(), m_jsonvec_sort.end(),
                  []( const Context& a, const Context& b) {
                      if(a.cur_indent != b.cur_indent)
                        return a.cur_indent > b.cur_indent;
                      else
                        return a.cur_index < b.cur_index;
                  });

        // 遍历 Context 向量
        for (const auto& ctx : m_jsonvec_sort) {
            // 查找当前层级对应的 JSON 对象
            auto it = std::find_if(m_jsonvec.begin(), m_jsonvec.end(),
                [&,ctx](const Context& item) {
                    return item.key == ctx.key
                            && item.cur_indent == ctx.cur_indent
                            && item.prant_json_index == ctx.prant_json_index
                            && item.cur_index == ctx.cur_index
                            ;
                }
            );
            Context& foundContext = *it;
            if (foundContext.cur_indent == 0) {
    //            j = foundContext.this_json;
            }
            else
            {
                int p_index =  foundContext.prant_json_index;
                if(m_jsonvec[p_index].is_array)
                {
//                    if(m_jsonvec[p_index].this_json["value"] == "no_item")
//                        m_jsonvec[p_index].this_json.erase("value");
                    m_jsonvec[p_index].this_json["value"].push_back(foundContext.this_json);
                    m_jsonvec[p_index].this_json["type"]=foundContext.type; //将子元素类型作为列表类型，以解决列表为空时丢失列表类型信息
                }
                else if(m_jsonvec[p_index].is_map)
                {
                    continue;
                }
                else
                {
                    m_jsonvec[p_index].this_json["value"][foundContext.key] = foundContext.this_json;
                }
            }
        }

//        for (int i=0; i<m_jsonvec.size(); i++) {
//            if(m_jsonvec[i].is_array)
//            {
//                std::string type = m_jsonvec[i].this_json["type"];
//                if (type == "std::set" ||type == "T[N]" ||type == "std::deque"||type == "std::list"||type == "std::vector" )
//                {
//                    if(!m_jsonvec[i].this_json.contains("value"))
//                    {
//                        m_jsonvec[i].this_json["type"] = "obj";
//                        m_jsonvec[i].this_json["value"] = "no item";
//                    }
//                }
//            }
//        }
    }

public:

private:
    std::vector<Context> m_jsonvec;
    int m_pjsonindex[128];
};

#endif // CPRINT2JSON_H
