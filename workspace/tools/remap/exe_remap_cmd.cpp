#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::map<std::string, std::string> GetRemapInfo(const std::string &file_name) {
    std::map<std::string, std::string> remap_map;
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << file_name << std::endl;
        return remap_map;
    }

    std::string line;
    while (std::getline(file, line)) {
        // 跳过空行
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string key, value;

        // 使用getline分割字符串
        if (std::getline(ss, key, ',')) {
            // 去除key前后空格
            size_t start = key.find_first_not_of(" \t");
            size_t end = key.find_last_not_of(" \t");
            if (start != std::string::npos) {
                key = key.substr(start, end - start + 1);
            }

            if (std::getline(ss, value)) {
                // 去除value前后空格
                start = value.find_first_not_of(" \t");
                end = value.find_last_not_of(" \t");
                if (start != std::string::npos) {
                    value = value.substr(start, end - start + 1);
                }

                remap_map[key] = value;
            }
        }
    }

    file.close();
    return remap_map;
}

int main(int argc, const char *argv[])
{
    // std::cout << "argc:" << argc << std::endl;

    int index = argc - 1;
    std::string file_name = std::string(argv[index]);
    // std::cout << "file name:" << file_name << std::endl;
    auto map_str = GetRemapInfo(file_name);

    std::string cmd;
    for (int i = 1; i < index; i++) {
        auto it = map_str.find(std::string(argv[i]));
        if (it == map_str.end()) {
            cmd += std::string(argv[i]) + " ";
        } else {
            cmd += it->second + " ";
        }
    }

    std::cout << "cmd is:" <<  cmd << std::endl;
    system(cmd.c_str());
    return 0;
}