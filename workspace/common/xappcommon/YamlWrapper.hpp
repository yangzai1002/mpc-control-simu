//
// Created by steve on 10/27/21.
//

#ifndef YAML_WRAPPER_HPP
#define YAML_WRAPPER_HPP

#include <yaml-cpp/yaml.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <type_traits>

#include "StringUtils.hpp"
#include "log/log.h"

namespace app_common {
    class YamlWrapper {
    public:
        YamlWrapper(const std::string &yamlfile) : _yaml_path(yamlfile) {
            load_from_file();
        }

        template <typename V> void get_value(V &v, const std::string &route) noexcept(false) {
            static_assert(!std::is_const<V>::value, "should not be const");
            try {
                get(v, route);
            } catch (...) {
                throw std::runtime_error("invalid yaml node " + route);
            }
        }

        void get_value(std::string &v, const std::string &route) noexcept(false) {
            try {
                get(v, route);
                StringUtils::trim(v);
            } catch (...) {
                throw std::runtime_error("invalid yaml node " + route);
            }
        }

        template <typename V> V get_value(const std::string &route) noexcept(false) {
            try {
                return get<V>(route);
            } catch (...) {
                throw std::runtime_error("invalid yaml node " + route);
            }
        }

        template <typename V> V get_value(const std::string &route, V def) noexcept(false) {
            try {
                return get<V>(route);
            } catch (...) {
                return def;
            }
        }

        std::string get_value(const std::string &route) noexcept(false) {
            try {
                return StringUtils::trim(get<std::string>(route));
            } catch (...) {
                throw std::runtime_error("invalid yaml node " + route);
            }
        }

        template <typename V> void set_value(const std::string &route, const V &v) noexcept(false) {
            set(route, v);
        }

        template <typename V> void set_or_add_value(const std::string &route, const V &v) noexcept(false) {
            set_or_add(route, v);
        }

        template <typename V> void get(V &v, const std::string &node_path) noexcept(false) {
            static_assert(!std::is_const<V>::value, "should not be const");

            std::lock_guard<std::mutex> guard(mtx);
            auto                        iter = get_iter(node_path);
            v = iter->second.as<V>();
        }

        template <typename V> V get(const std::string &node_path) noexcept(false) {
            std::lock_guard<std::mutex> guard(mtx);

            auto iter = get_iter(node_path);
            return iter->second.as<V>();
        }

        template <typename V> void set(const std::string &node_path, const V &v) noexcept(false) {
            std::lock_guard<std::mutex> guard(mtx);

            auto iter = get_iter(node_path);
            iter->second = v;

            std::ofstream f(_yaml_path);
            f.exceptions(std::ios_base::failbit | std::ios_base::badbit);
            YAML::Emitter out(f);
            out.SetIndent(2);
            out << cfg;
        }

        template <typename V> void set_or_add(const std::string &node_path, const V &v) noexcept(false) {
            std::lock_guard<std::mutex> guard(mtx);

            auto iter = get_iter_add_if_none(node_path);
            iter->second = v;

            std::ofstream f(_yaml_path);
            f.exceptions(std::ios_base::failbit | std::ios_base::badbit);
            YAML::Emitter out(f);
            out.SetIndent(2);
            out << cfg;
        }

    private:
        void load_from_file() {
            try {
                auto p = YAML::LoadFile(_yaml_path);
                cfg = YAML::Clone(p);
            } catch (const std::runtime_error &e) {
                AERROR << "load " << _yaml_path << " failed with: " << e.what();
                std::rethrow_exception(std::current_exception());
            } catch (...) {
                AERROR << "load " << _yaml_path << " failed with: "
                       << "unknown";
                std::rethrow_exception(std::current_exception());
            }
        }

        static std::list<std::string> path_nodes(const std::string &node_path);

        YAML::Node get_node(const std::string &node_path);

        YAML::iterator get_iter(const std::string &node_path);

        YAML::iterator get_top_iter(const std::string &node);

        static YAML::iterator get_sub_iter(const YAML::iterator &iter, std::string &node);

        YAML::iterator get_iter_add_if_none(const std::string &node_path);

        YAML::iterator get_top_iter_add_if_none(const std::string &node);

        static YAML::iterator get_sub_iter_add_if_none(const YAML::iterator &iter, std::string &node);

    private:
        const std::string _yaml_path;
        std::mutex        mtx;
        YAML::Node        cfg;
    };
} // namespace app_common
#endif
