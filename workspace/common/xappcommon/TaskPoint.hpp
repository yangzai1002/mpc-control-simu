#ifndef TASK_POINT_HPP
#define TASK_POINT_HPP

#include <tinyxml2.h>

#include <cstdint>
#include <string>
#include <vector>

#include "JsonUtils.hpp"

struct TaskPoint {
    enum class PointStatus : std::uint32_t { IDLE, START, END };

    enum class PointType : std::uint32_t {
        INVALID = 0,
        TASK_POINT = 0x01,
        NARROW_IN = 0x02,
        NARROW_OUT = 0x04,
        SPECIAL_TAG = 0x08,
        POSTURE_POINT = 0x10,
        TRAFFIC_LIGHT_POINT = 0x20,
        NEW_TASK_POINT = 0x40,
        FLAT_POINT = 0x50,
        END_POINT = 0x60,
        PROPERTY_POINT = 0x80
    };

    struct PointTypeString {
        // const static std::string TASK_POINT;
        // const static std::string NARROW_IN;
        // const static std::string NARROW_OUT;
        // const static std::string SPECIAL_TAG;
        // const static std::string POSTURE_POINT;
        // const static std::string TRAFFIC_LIGHT_POINT;
        // const static std::string NEW_TASK_POINT;
        // const static std::string PROPERTY_POINT;

        // const std::string TaskPoint::PointTypeString::TASK_POINT =
        // "TASK_POINT"; const std::string TaskPoint::PointTypeString::NARROW_IN
        // = "NARROW_IN"; const std::string
        // TaskPoint::PointTypeString::NARROW_OUT = "NARROW_OUT"; const
        // std::string TaskPoint::PointTypeString::SPECIAL_TAG = "SPECIAL_TAG";
        // const std::string TaskPoint::PointTypeString::POSTURE_POINT =
        // "POSTURE_POINT"; const std::string
        // TaskPoint::PointTypeString::TRAFFIC_LIGHT_POINT =
        // "TRAFFIC_LIGHT_POINT"; const std::string
        // TaskPoint::PointTypeString::NEW_TASK_POINT = "NEW_TASK_POINT"; const
        // std::string TaskPoint::PointTypeString::PROPERTY_POINT =
        // "PROPERTY_POINT";

        static const constexpr char* TASK_POINT = "TASK_POINT";
        static const constexpr char* NARROW_IN = "NARROW_IN";
        static const constexpr char* NARROW_OUT = "NARROW_OUT";
        static const constexpr char* SPECIAL_TAG = "SPECIAL_TAG";
        static const constexpr char* POSTURE_POINT = "POSTURE_POINT";
        static const constexpr char* TRAFFIC_LIGHT_POINT = "TRAFFIC_LIGHT_POINT";
        static const constexpr char* NEW_TASK_POINT = "NEW_TASK_POINT";
        static const constexpr char* PROPERTY_POINT = "PROPERTY_POINT";
    };

    int         id;
    std::string name;
    float       high;
    float       pitch;
    float       yaw;
    int         zoom;
    double      lat;
    double      lon;
    double      heading;
    PointType   type;         // 类型可以同时有多个，做位与
    int         startend_set; // 起始终止点设置 0：普通，1：起始点，2：终止点
    int         speed;
    int         avoid_status;
    int         perception_status;
    std::string car_type;
    int         task_type;
    std::string task_area;
    bool        is_stop;
    int         task_time;
    bool        end_point;
    std::string travel_mode;

    TaskPoint()
        : id(0)
        , high(0)
        , pitch(0)
        , yaw(0)
        , zoom(0)
        , lat(0)
        , lon(0)
        , heading(0)
        , type(PointType::INVALID)
        , startend_set(0)
        , speed(0)
        , avoid_status(0)
        , perception_status(0)
        , task_type(0)
        , is_stop(false)
        , task_time(0)
        , end_point(false) {
    }

    explicit TaskPoint(const tinyxml2::XMLElement* e) : TaskPoint() {
        if (nullptr == e) {
            return;
        }
        xml2taskpoint(*this, type, e);
    }

    explicit TaskPoint(const nlohmann::json& json) : TaskPoint() {
        // TODO: 必须性、关联性校验
        app_common::JsonUtils::valueFromJson(id, json, "id", true);
        app_common::JsonUtils::valueFromJson(name, json, "name", false, std::to_string(id));

        app_common::JsonUtils::valueFromJson(lat, json, "lat", false);
        app_common::JsonUtils::valueFromJson(lon, json, "lon", false);
        app_common::JsonUtils::valueFromJson(yaw, json, "yaw", false);

        std::string              type_str;
        std::vector<std::string> type_vec;
        app_common::JsonUtils::valueFromJson(type_str, json, "type", false);
        type_vec.emplace_back(type_str);
        type = TaskPoint::TypeStringVecToInt(type_vec);

        app_common::JsonUtils::valueFromJson(speed, json, "speed", false);
        app_common::JsonUtils::valueFromJson(avoid_status, json, "avoid_status", false);
        app_common::JsonUtils::valueFromJson(perception_status, json, "perception_status", false);
        app_common::JsonUtils::valueFromJson(car_type, json, "car_type", false);
        app_common::JsonUtils::valueFromJson(task_type, json, "task_type", false);
        app_common::JsonUtils::valueFromJson(task_area, json, "task_area", false);
        if (task_area == "") {
            task_area = "null";
        }
        app_common::JsonUtils::valueFromJson(heading, json, "heading", false);
        app_common::JsonUtils::valueFromJson(task_time, json, "task_time", false);
        app_common::JsonUtils::valueFromJson(is_stop, json, "is_stop", false, false);
        app_common::JsonUtils::valueFromJson(end_point, json, "end_point", false, false);
        app_common::JsonUtils::valueFromJson(travel_mode, json, "travel_mode", false, "null");
        if (travel_mode == "") {
            travel_mode = "null";
        }
    }

    tinyxml2::XMLElement* to_xml_element(tinyxml2::XMLDocument& doc, std::int32_t new_id) {
        auto wpt = doc.NewElement("wpt");
        if (nullptr == wpt) {
            return nullptr;
        }

        wpt->SetAttribute("lat", lat);
        wpt->SetAttribute("lon", lon);
        wpt->SetAttribute("heading", heading);
        wpt->SetAttribute("type", (std::uint32_t)type);
        wpt->SetAttribute("id", new_id);

        auto e = doc.NewElement("name");
        e->SetText(name.c_str());
        wpt->InsertEndChild(e);

        e = doc.NewElement("high");
        e->SetText(high);
        wpt->InsertEndChild(e);

        e = doc.NewElement("pitch");
        e->SetText(pitch);
        wpt->InsertEndChild(e);

        e = doc.NewElement("yaw");
        e->SetText(yaw);
        wpt->InsertEndChild(e);

        e = doc.NewElement("zoom");
        e->SetText(zoom);
        wpt->InsertEndChild(e);

        e = doc.NewElement("startend_set");
        e->SetText(startend_set);
        wpt->InsertEndChild(e);

        e = doc.NewElement("speed");
        e->SetText(speed);
        wpt->InsertEndChild(e);

        e = doc.NewElement("avoid_status");
        e->SetText(avoid_status);
        wpt->InsertEndChild(e);

        e = doc.NewElement("perception_status");
        e->SetText(perception_status);
        wpt->InsertEndChild(e);

        e = doc.NewElement("car_type");
        e->SetText(car_type.c_str());
        wpt->InsertEndChild(e);

        e = doc.NewElement("task_type");
        e->SetText(task_type);
        wpt->InsertEndChild(e);

        e = doc.NewElement("task_area");
        e->SetText(task_area.c_str());
        wpt->InsertEndChild(e);

        e = doc.NewElement("is_stop");
        e->SetText(is_stop);
        wpt->InsertEndChild(e);

        e = doc.NewElement("end_point");
        e->SetText(end_point);
        wpt->InsertEndChild(e);

        e = doc.NewElement("task_time");
        e->SetText(task_time);
        wpt->InsertEndChild(e);

        e = doc.NewElement("travel_mode");
        e->SetText(travel_mode.c_str());
        wpt->InsertEndChild(e);

        return wpt;
    };

    // 转字符串数组
    static std::vector<std::string> TypeIntToStringVec(PointType pt) {
        auto                     intdata = (std::uint32_t)pt;
        std::vector<std::string> stringVec;
        if (intdata & (std::uint32_t)PointType::TASK_POINT) {
            stringVec.push_back("TASK_POINT");
        }
        if (intdata & (std::uint32_t)PointType::NARROW_IN) {
            stringVec.push_back("NARROW_IN");
        }
        if (intdata & (std::uint32_t)PointType::NARROW_OUT) {
            stringVec.push_back("NARROW_OUT");
        }
        if (intdata & (std::uint32_t)PointType::SPECIAL_TAG) {
            stringVec.push_back("SPECIAL_TAG");
        }
        if (intdata & (std::uint32_t)PointType::POSTURE_POINT) {
            stringVec.push_back("POSTURE_POINT");
        }
        if (intdata & (std::uint32_t)PointType::TRAFFIC_LIGHT_POINT) {
            stringVec.push_back("TRAFFIC_LIGHT_POINT");
        }
        if (intdata & (std::uint32_t)PointType::PROPERTY_POINT) {
            stringVec.push_back("PROPERTY_POINT");
        }

        return stringVec;
    }

    // 字符串数组转
    static PointType TypeStringVecToInt(const std::vector<std::string>& stringVec) {
        std::uint32_t type = 0;
        for (auto str : stringVec) {
            if (str == PointTypeString::TASK_POINT) {
                type = type | (std::uint32_t)PointType::TASK_POINT;
            } else if (str == PointTypeString::NARROW_IN) {
                type = type | (std::uint32_t)PointType::NARROW_IN;
            } else if (str == PointTypeString::NARROW_OUT) {
                type = type | (std::uint32_t)PointType::NARROW_OUT;
            } else if (str == PointTypeString::SPECIAL_TAG) {
                type = type | (std::uint32_t)PointType::SPECIAL_TAG;
            } else if (str == PointTypeString::POSTURE_POINT) {
                type = type | (std::uint32_t)PointType::POSTURE_POINT;
            } else if (str == PointTypeString::TRAFFIC_LIGHT_POINT) {
                type = type | (std::uint32_t)PointType::TRAFFIC_LIGHT_POINT;
            } else if (str == PointTypeString::PROPERTY_POINT) {
                type = type | (std::uint32_t)PointType::PROPERTY_POINT;
            }
        }
        return (PointType)type;
    }

    template <typename T> static void xml2taskpoint(T& p, PointType& type, const tinyxml2::XMLElement* e) {
        if (nullptr == e) {
            return;
        }

        // TODO: 文件格式保护
        p.lat = e->DoubleAttribute("lat");
        p.lon = e->DoubleAttribute("lon");
        p.heading = e->DoubleAttribute("heading");
        // p.type = (PointType)e->IntAttribute("type");
        type = (PointType)e->IntAttribute("type");
        p.id = e->IntAttribute("id");

        auto c = e->FirstChildElement("name");
        if (c != nullptr && c->GetText() != nullptr) {
            p.name = c->GetText();
        }

        c = e->FirstChildElement("high");
        if (c != nullptr) {
            p.high = c->DoubleText();
        }

        c = e->FirstChildElement("pitch");
        if (c != nullptr) {
            p.pitch = c->DoubleText();
        }

        c = e->FirstChildElement("yaw");
        if (c != nullptr) {
            p.yaw = c->FloatText();
        }

        c = e->FirstChildElement("zoom");
        if (c != nullptr) {
            p.zoom = c->IntText();
        }

        c = e->FirstChildElement("speed");
        if (c != nullptr) {
            p.speed = c->IntText();
        }

        c = e->FirstChildElement("avoid_status");
        if (c != nullptr) {
            p.avoid_status = c->IntText();
        }

        c = e->FirstChildElement("perception_status");
        if (c != nullptr) {
            p.perception_status = c->IntText();
        }

        c = e->FirstChildElement("car_type");
        if (c != nullptr && c->GetText() != nullptr) {
            p.car_type = c->GetText();
        }

        c = e->FirstChildElement("task_type");
        if (c != nullptr) {
            p.task_type = c->IntText();
        }

        c = e->FirstChildElement("task_area");
        if (c != nullptr && c->GetText() != nullptr) {
            p.task_area = c->GetText();
        }

        c = e->FirstChildElement("is_stop");
        if (c != nullptr) {
            p.is_stop = c->BoolText();
        }

        c = e->FirstChildElement("task_time");
        if (c != nullptr) {
            p.task_time = c->IntText();
        }

        c = e->FirstChildElement("end_point");
        if (c != nullptr) {
            p.end_point = c->BoolText();
        }

        c = e->FirstChildElement("travel_mode");
        if (c != nullptr && c->GetText() != nullptr) {
            p.travel_mode = c->GetText();
        }

        auto tmp = e->FirstChildElement("startend_set");
        if (tmp == nullptr) {
            p.startend_set = 0;
        } else {
            p.startend_set = e->FirstChildElement("startend_set")->IntText();
        }
    }
};
using TaskPoints = std::vector<TaskPoint>;

#endif
