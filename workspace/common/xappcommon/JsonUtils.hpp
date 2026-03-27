//
// Created by yixingwang on 2023/11/22.
//

#ifndef COMMON_UTILS_JSON_UTILS_HPP
#define COMMON_UTILS_JSON_UTILS_HPP

#include <cstdint>
#include <iostream>
#include <list>
#include <optional>
#include <string>

#include "json.hpp"

namespace app_common {
    class JsonUtils {
    private:
        JsonUtils() = delete;

    public:
        static void should_be_array(const nlohmann::json& d) noexcept(false) {
            if (!d.is_array()) {
                throw std::runtime_error("SHOULD_BE_ARRAY: " + d.dump());
            }
        };

        static void should_be_object(const nlohmann::json& d) noexcept(false) {
            if (!d.is_object()) {
                throw std::runtime_error("SHOULD_BE_OBJECT: " + d.dump());
            }
        }

        static void field_should_be_array(const nlohmann::json& d, const std::string& key) noexcept(false) {
            should_be_object(d);

            try {
                auto& f = d.at(key);
                if (!f.is_array()) {
                    JsonUtils::throwFieldFormatError(key, d, "should_be_array");
                }
            } catch (const nlohmann::detail::out_of_range& e) {
                JsonUtils::throwFieldMissingError(key, d);
            }
        }

        static void field_should_be_object(const nlohmann::json& d, const std::string& key) noexcept(false) {
            should_be_object(d);

            try {
                auto& f = d.at(key);
                if (!f.is_object()) {
                    JsonUtils::throwFieldFormatError(key, d, "should_be_object");
                }
            } catch (const nlohmann::detail::out_of_range& e) {
                JsonUtils::throwFieldMissingError(key, d);
            }
        }

        static void rawFromJson(std::string& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                const std::string& def = "") noexcept(false) {
            should_be_object(d);
            try {
                auto& f = d.at(k);
                v = f.dump();
            } catch (const nlohmann::detail::out_of_range& e) {
                if (mandatory) {
                    throwFieldMissingError(k, d);
                } else {
                    v = def;
                }
            }
        }

        static void valueFromJson(std::optional<std::string>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            should_be_object(d);
            try {
                auto& f = d.at(k);
                if (!f.is_string()) {
                    throwFieldFormatError(k, d, "shoule_be_string");
                }
                v = f.get<std::string>();
            } catch (const nlohmann::detail::out_of_range& e) {
            }
        }

        static void valueFromJson(std::optional<bool>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            should_be_object(d);
            try {
                auto& f = d.at(k);
                if (!f.is_boolean()) {
                    throwFieldFormatError(k, d, "shoule_be_boolean");
                }
                v = f.get<bool>();
            } catch (const nlohmann::detail::out_of_range& e) {
            }
        }

        template <typename T> static void valueIntFromJson(std::optional<T>& v, const nlohmann::json& d,
                                                           const std::string& k) noexcept(false) {
            should_be_object(d);
            try {
                auto& f = d.at(k);
                if (!f.is_number_integer()) {
                    throwFieldFormatError(k, d, "shoule_be_integer");
                }
                auto t = f.get<T>();
                v = t;
            } catch (const nlohmann::detail::out_of_range& e) {
            }
        }

        template <typename T> static void valueUintFromJson(std::optional<T>& v, const nlohmann::json& d,
                                                            const std::string& k) noexcept(false) {
            should_be_object(d);
            try {
                auto& f = d.at(k);
                if (!f.is_number_unsigned()) {
                    throwFieldFormatError(k, d, "shoule_be_unsigned");
                }
                auto t = f.get<T>();
                v = t;
            } catch (const nlohmann::detail::out_of_range& e) {
            }
        }

        template <typename T>
        static void valueFloatFromJson(std::optional<T>& v, const nlohmann::json& d, const std::string& k) {
            should_be_object(d);

            try {
                auto& f = d.at(k);
                if (!f.is_number()) {
                    throwFieldFormatError(k, d, "shoule_be_number");
                }
                auto t = f.get<T>();
                v = t;
            } catch (const nlohmann::detail::out_of_range& e) {
            }
        }

        static void valueFromJson(std::optional<std::int64_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueIntFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::int32_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueIntFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::int16_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueIntFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::int8_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueIntFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::uint64_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueUintFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::uint32_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueUintFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::uint16_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueUintFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<std::uint8_t>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueUintFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<double>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueFloatFromJson(v, d, k);
        }

        static void valueFromJson(std::optional<float>& v, const nlohmann::json& d,
                                  const std::string& k) noexcept(false) {
            valueFloatFromJson(v, d, k);
        }

        static void valueFromJson(std::string& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  const std::string& def = "") noexcept(false) {
            std::optional<std::string> tv;
            valueFromJson(tv, d, k);
            if (!tv.has_value()) {
                if (mandatory) {
                    throwFieldMissingError(k, d);
                } else {
                    v = def;
                }
            } else {
                v = *tv;
            }
        }

        static void valueFromJson(bool& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  const bool def = false) noexcept(false) {
            std::optional<bool> tv;
            valueFromJson(tv, d, k);
            if (!tv) {
                if (mandatory) {
                    throwFieldMissingError(k, d);
                } else {
                    v = def;
                }
            } else {
                v = *tv;
            }
        }

        static void valueFromJson(std::int64_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::int64_t def = 0) noexcept(false) {
            valueIntFromJson(v, d, k, def);
        }

        static void valueFromJson(std::int32_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::int32_t def = 0) noexcept(false) {
            valueIntFromJson(v, d, k, def);
        }

        static void valueFromJson(std::int16_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::int16_t def = 0) noexcept(false) {
            valueIntFromJson(v, d, k, def);
        }

        static void valueFromJson(std::int8_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::int8_t def = 0) noexcept(false) {
            valueIntFromJson(v, d, k, def);
        }

        static void valueFromJson(std::uint64_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::uint64_t def = 0) noexcept(false) {
            valueUintFromJson(v, d, k, def);
        }

        static void valueFromJson(std::uint32_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::uint32_t def = 0) noexcept(false) {
            valueUintFromJson(v, d, k, def);
        }

        static void valueFromJson(std::uint16_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::uint16_t def = 0) noexcept(false) {
            valueUintFromJson(v, d, k, def);
        }

        static void valueFromJson(std::uint8_t& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  std::uint8_t def = 0) noexcept(false) {
            valueUintFromJson(v, d, k, def);
        }

        static void valueFromJson(double& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  double def = 0) noexcept(false) {
            valueFloatFromJson(v, d, k, def);
        }

        static void valueFromJson(float& v, const nlohmann::json& d, const std::string& k, bool mandatory,
                                  float def = 0) noexcept(false) {
            valueFloatFromJson(v, d, k, def);
        }

        template <typename T> static void valueIntFromJson(T& v, const nlohmann::json& d, const std::string& k,
                                                           bool mandatory, T def = 0) noexcept(false) {
            std::optional<T> tv;
            valueIntFromJson(tv, d, k);
            if (!tv.has_value()) {
                if (mandatory) {
                    throwFieldMissingError(k, d);
                } else {
                    v = def;
                }
            } else {
                v = *tv;
            }
        }

        template <typename T> static void valueUintFromJson(T& v, const nlohmann::json& d, const std::string& k,
                                                            bool mandatory, T def = 0) noexcept(false) {
            std::optional<T> tv;
            valueUintFromJson(tv, d, k);
            if (!tv.has_value()) {
                if (mandatory) {
                    throwFieldMissingError(k, d);
                } else {
                    v = def;
                }
            } else {
                v = *tv;
            }
        }

        template <typename T>
        static void valueFloatFromJson(T& v, const nlohmann::json& d, const std::string& k, bool mandatory, T def = 0) {
            std::optional<T> tv;

            valueFloatFromJson(tv, d, k);

            if (!tv.has_value()) {
                if (mandatory) {
                    throwFieldMissingError(k, d);
                } else {
                    v = def;
                }
            } else {
                v = *tv;
            }
        }

        static void throwFieldMissingError(const std::string& field, const nlohmann::json& json) {
            throw std::runtime_error("field " + field + " should be an element of " + json.dump());
        }

        static void throwFieldMissingError(const std::string& field, const std::string& json) {
            throw std::runtime_error("field " + field + " should be an element of " + json);
        };

        static void throwFieldFormatError(const std::string& field, const nlohmann::json& doc, const std::string& why) {
            throw std::runtime_error("field " + field + " of " + doc.dump() + " format error because of " + why);
        }

        static void throwArrayElementFormatError(const std::string& field, const nlohmann::json& element,
                                                 const std::string& why) {
            throw std::runtime_error("field " + field + "'s element " + element.dump() + " format error because of " +
                                     why);
        }

        static void throwArrayElementFormatError(const nlohmann::json& element, const std::string& why) {
            throw std::runtime_error("element " + element.dump() + " format error because of " + why);
        }
    };
};     // namespace app_common
#endif // POSTSAMPLE_PROCESSOR_PERSISTENCE_JSONUTIL_HPP
