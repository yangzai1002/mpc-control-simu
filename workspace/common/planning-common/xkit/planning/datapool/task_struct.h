#ifndef TASK_STRUCT_H__
#define TASK_STRUCT_H__
#include "TaskPoint.hpp"

namespace avos {
    namespace planning {
        //跟随属性
        struct FollowProperty {
        bool avoid;
        char follow_type;
        char follow_mode;
        double safe_distance;
        double max_speed;
        FollowProperty() {
            Reset();
        }
        void Reset() {
            avoid = true;
            follow_type = 0;
            follow_mode = 0;
            safe_distance = 0;
            max_speed = 0.0;
        }
        };

        //跟随目标
        struct TargetPoint {
        int id;
        char target_origin;
        double lon;
        double lat;
        double x;
        double y;
        double heading;
        double speed;

        TargetPoint() {
            Reset();
        }
        void Reset() {
            id = -1;
            target_origin = 0;
            lon = 0.0;
            lat = 0.0;
            x = 0.0;
            y = 0.0;
            heading = 0.0;
            speed = 0.0;
        }
        };

        //跟随目标信息
        // struct FollowTarget {
        // bool invalid;
        // FollowProperty follow_property;
        // TargetPoint targetpoint;
        // FollowTarget() {
        //     Reset();
        // }
        // void Reset() {
        //     invalid = true;
        //     follow_property.Reset();
        //     targetpoint.Reset();
        // }
        // };



        struct TaskProperty {
            bool invalid;
            double max_speed;
            bool max_speed_invalid;
            int avoid;
            bool avoid_invalid;
            std::string undifined;
            int perception_status;
            bool perception_status_invalid;
            
            TaskProperty() {
                Reset();
            }
            void Reset() {
                invalid = false;
                max_speed = 0.0;
                max_speed_invalid = true;
                avoid = 1;
                avoid_invalid = true;
                undifined = "NONE";
                perception_status = 0;
                perception_status_invalid = true;
            }
        };

        struct GPSPoint {
        double lat;
        double lon;
        double heading;
        GPSPoint() { Reset(); }
        void Reset() {
            lat = 0.0;
            lon = 0.0;
            heading = 0.0;
        }
        void Set(double _lat, double _lon, double _heading) {
            lat = _lat;
            lon = _lon;
            heading = _heading;
        }
        };

        struct InnerPoint {
        double x;
        double y;
        double angle;
        InnerPoint() { Reset(); }
        void Reset() {
            x = 0.0;
            y = 0.0;
            angle = 0.0;
        }
        void Set(double _x, double _y, double _angle) {
            x = _x;
            y = _y;
            angle = _angle;
        }
        };

        struct InnerGlobalPoint {
        double xg;
        double yg;
        double global_angle;
        InnerGlobalPoint() { Reset(); }
        void Reset() {
            xg = 0.0;
            yg = 0.0;
            global_angle = 0.0;
        }
        void Set(double _xg, double _yg, double _global_angle) {
            xg = _xg;
            yg = _yg;
            global_angle = _global_angle;
        }
        };

        //输入点类型
        struct InputPoint {
        GPSPoint gps_point;
        int index;
        bool avoid;
        double speed;
        char direction;
        double length;
        double dis_fence;
        InputPoint() {
            Reset();
        }
        void Reset() {
            gps_point.Reset();
            index = -1;
            avoid = false;
            speed = 0.0;
            direction = 0;
            length = 0.0;
            dis_fence = 1.45;
        }
        };


        //输入的点信息
        struct InputPath {
        double timestamp;
        bool invalid;
        std::vector<InputPoint> path;
        InputPath() {
            Reset();
        }
        void Reset() {
            timestamp = 0.0;
            invalid = true;
            path.clear();
        }
        };

        //内部用的点信息
        struct InnerPointInfo {
        InnerPoint point;
        InnerGlobalPoint global_point;
        int index;
        bool avoid;
        double speed;
        double length;
        int direction;
        double curvature;
        double dis_fence;
        InnerPointInfo() { Reset(); }
        void Reset() {
            point.Reset();
            global_point.Reset();
            index = -1;
            avoid = false;
            speed = 0.0;
            length = 0.0;
            direction = 0;
            curvature = 0;
            dis_fence = 1.45;
        }
        };

        struct InnerPath {
        double timestamp;
        int status;
        int avoid;
        double speed;
        std::vector<InnerPointInfo> path;
        InnerPath() {
            Reset();
        }
        void Reset() {
            timestamp = 0.0;
            status = 0;
            avoid = 1;
            speed = 0.0;
            path.clear();
        }
        };

        //循迹任务点
        struct TrackTaskPoint {
        GPSPoint gps_point;
        double x;
        double y;
        double angle;
        double xg;
        double yg;
        double angleg;
        std::string name;
        int task_time;
        TaskPoint::PointType type;
        float high;
        float pitch;
        float yaw;
        int zoom;
        bool valid;
        bool task_finished;
        bool task_executing;
        bool destination;
        TrackTaskPoint() {
            Reset();
        }
        void Reset() {
            gps_point.Reset();
            x = 0.0;
            y = 0.0;
            angle = 0.0;
            xg = 0.0;
            yg = 0.0;
            angleg = 0.0;
            name = "";
            task_time = 0;
            type = TaskPoint::PointType::TASK_POINT;
            high = 0.0;
            pitch = 0.0;
            yaw = 0.0;
            zoom = 0;
            valid = false;
            task_finished = false;
            task_executing = false;
            destination = true;
        }

        // void SetTaskPoint(const double& x_, const double& y_, const double& angle_,
        //                   const TaskPoint::PointType& type_, const bool& destination_ = false) {
        //     x = x_;
        //     y = y_;
        //     angle = angle_;
        //     type = type_;
        //     destination = destination_;
        //     valid = true;
        // }
        };

        struct PayloadTaskInfo {
            bool flat_adjust;
            // bool heading_adjust;
            // double lat;
            // double lon;
            // double dir;

            PayloadTaskInfo() {
                flat_adjust = false;
                // bool heading_adjust = false;
                // double lat = 0.0;
                // double lon = 0.0;
                // double dir = 0.0;
            }
        };

        struct PayloadTaskStatus {
            int status;

            PayloadTaskStatus() {
                status = 0;
            }
        };
    }
}


#endif
