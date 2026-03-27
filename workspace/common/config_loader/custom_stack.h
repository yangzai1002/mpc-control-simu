#ifndef __CUSTOM_STACK_H__
#define __CUSTOM_STACK_H__
#include <errno.h>
#include <pthread.h>
#include <pthread.h> // pthread_self / pthread_setaffinity_np
#include <sched.h>
#include <sched.h> // CPU_ZERO / CPU_SET / sched_setscheduler
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

#include <chrono>
#include <deque>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>

#include "param_define.h"
#include "project_config.h"

class CustomStack {
private:
    CustomStack() {
        application_path = "";
        node_config_path = "";
    }

public:
    void SetProjectConfigPath(const std::string &path) {
        application_path = path;
    }

    std::string GetProjectConfigPath() {
        return application_path;
    }

    void SetNodeConfigPath(const std::string &path) {
        node_config_path = path;
    }

    // std::string GetNodeConfigPath() {
    //  return node_config_path;
    // }
    std::string GetNodeConfigPath() {
        // std::string usr_config_path = CONFIG_PATH;
        // std::cout << "usr node configpath = " << usr_config_path << std::endl;
        // node_config_path = usr_config_path+"/config";
        return node_config_path;
    }

    std::string GetVehicleConfigPath() {
        return GetProjectConfigValue<std::string>("vehicle_config_path");
    }

    std::string GetVehicleVinPath() {
        return GetProjectConfigValue<std::string>("vin_file");
    }

    static CustomStack *Instance() {
        static CustomStack *in = nullptr;
        if (in == nullptr) {
            in = new CustomStack();
        }
        return in;
    }

    void LoadAllProjectConfig(const std::string &path) {
        auto path_vec = Tools::TraverseDirectory(path);
        for (auto &p : path_vec) {
            LoadProjectConfig(p);
        }
    }

    void LoadModuleConfig(const std::string &path) {
        if (module_config.DeserializationFromFile(path) != 0) {
            std::cout << "cannot get load config " << path << std::endl;
            // exit(0);
            return;
        }
        module_config.Print();
    }

    void LoadProjectConfig(const std::string &path) {
        std::string str = Tools::GetFileExtension(path);
        std::cout << "path: " << path << ", " << str << std::endl;

        if (str != "proj_cfg") {
            return;
        }
        project_config.loadFile(path);
    }

    int GetConfig(const std::string &module_name, const std::string &param_name, std::string &value) {
        return module_config.GetParam(module_name, param_name, value);
    }

    int GetConfig(const std::string &module_name, const std::string &param_name, bool &value) {
        std::string val;
        if (module_config.GetParam(module_name, param_name, val) == 0) {
            if (val == "true" || val == "True") {
                value = true;
            } else {
                value = false;
            }
            return 0;
        }
        value = false;
        return 0;
    }

    template <typename T> int GetConfig(const std::string &module_name, const std::string &param_name, T &value) {
        std::string val;
        if (module_config.GetParam(module_name, param_name, val) == 0) {
            std::stringstream ss;
            ss << val;
            ss >> value;
            return 0;
        }
        return -1;
    }

    template <typename T> bool GetProjectConfigValue(const std::string &key, T &data) {
        return project_config.getValue<T>(key, data);
    }

    template <typename T> T GetProjectConfigValue(const std::string &key) {
        return project_config.getValue<T>(key);
    }

    template <typename T> bool GetProjectConfigArray(const std::string &key, std::vector<T> &array) {
        return project_config.getArray<T>(key, array);
    }

    void LoadOtherProjectConfig() {
        project_config.CheckOtherConfig();
    }

public:
    std::string application_path;
    std::string node_config_path;

    ModuleConfig      module_config;
    ProjectYamlConfig project_config;
};

#endif
