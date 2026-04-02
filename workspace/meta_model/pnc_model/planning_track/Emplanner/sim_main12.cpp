#include <iostream>
#include <vector>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <cmath>

#include "planner_interface.h"
#include "simulation/vehicle_simulator.h"

using namespace localplanner;
using namespace localplanner::interface;

// ==========================================
// 严谨的 UTM 转经纬度 (彻底消除投影漂移)
// ==========================================
void SimUTMToLatLon(double x, double y, double& lat, double& lon) {
    double a = 6378137.0;
    double f = 1.0 / 298.257223563;
    double b = a * (1 - f);
    double e = std::sqrt(1 - (b * b) / (a * a));
    double e2 = e * e;
    double e_prime_2 = e2 / (1 - e2);

    int zone = 51; // 假设在 51带
    double x_val = x - 500000.0;
    double y_val = y;

    double M = y_val / 0.9996;
    double mu = M / (a * (1 - e2 / 4 - 3 * e2 * e2 / 64 - 5 * e2 * e2 * e2 / 256));

    double e1 = (1 - std::sqrt(1 - e2)) / (1 + std::sqrt(1 - e2));
    double phi1 = mu + (3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * std::sin(2 * mu)
                  + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * std::sin(4 * mu)
                  + (151 * e1 * e1 * e1 / 96) * std::sin(6 * mu);

    double N1 = a / std::sqrt(1 - e2 * std::sin(phi1) * std::sin(phi1));
    double T1 = std::tan(phi1) * std::tan(phi1);
    double C1 = e_prime_2 * std::cos(phi1) * std::cos(phi1);
    double R1 = a * (1 - e2) / std::pow(1 - e2 * std::sin(phi1) * std::sin(phi1), 1.5);
    double D = x_val / (N1 * 0.9996);

    double lat_rad = phi1 - (N1 * std::tan(phi1) / R1) * (D * D / 2
                     - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e_prime_2) * D * D * D * D / 24
                     + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e_prime_2 - 3 * C1 * C1) * D * D * D * D * D * D / 720);

    double lon_0 = (zone - 1) * 6 - 180 + 3;
    double lon_rad = lon_0 * M_PI / 180.0 + (D - (1 + 2 * T1 + C1) * D * D * D / 6
                     + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e_prime_2 + 24 * T1 * T1) * D * D * D * D * D / 120) / std::cos(phi1);

    lat = lat_rad * 180.0 / M_PI;
    lon = lon_rad * 180.0 / M_PI;
}

const int CANVAS_W = 1000, CANVAS_H = 1000, PANEL_W = 400;
const double SCALE = 5.0; 

void DrawVehicleHUD(cv::Mat& img, const cv::Point& center, double heading_rad) {
    double l = 4.8 * SCALE, w = 2.0 * SCALE; 
    cv::RotatedRect car_rect(center, cv::Size2f(l, w), -heading_rad * 180.0 / M_PI);
    cv::Point2f pts[4];
    car_rect.points(pts);
    for (int i = 0; i < 4; i++) cv::line(img, pts[i], pts[(i + 1) % 4], {255, 255, 255}, 2, cv::LINE_AA);
    cv::Point arrow = center + cv::Point(std::cos(heading_rad) * l * 0.6, -std::sin(heading_rad) * l * 0.6);
    cv::arrowedLine(img, center, arrow, {0, 0, 255}, 2, cv::LINE_AA);
}

struct SimObs { double x, y, w, h; };

int main() {
    std::cout << ">>> [TitanPlanner] Perfect UTM Simulation Bridge Booting..." << std::endl;
    PlannerInterface planner_interface;

    // 锚定一个真实的 UTM 大坐标
    const double START_UTM_X = 500000.0;
    const double START_UTM_Y = 3450000.0;

    simulation::VehicleSimulator::Config sim_cfg;
    simulation::VehicleSimulator::State sim_state;
    sim_state.x = START_UTM_X;
    sim_state.y = START_UTM_Y;
    sim_state.theta = std::atan2(10.0, 2.0); // 完美对齐路径初始方向！消除扭动！
    sim_state.v = 0.0;            
    simulation::VehicleSimulator simulator(sim_cfg, sim_state);

    std::vector<std::pair<double, double>> sim_global_path_xy;
    for (int i = 0; i < 30; ++i) {
        sim_global_path_xy.push_back({START_UTM_X + i * 2.0, START_UTM_Y + i * 10.0});
    }

    std::vector<SimObs> sim_obstacles = {
        // 障碍物 1：在参考线右侧 (Y=40 时参考线 X=8)，迫使车辆向左避让
        {START_UTM_X + 9.0, START_UTM_Y + 40.0, 2.5, 2.5}, 

        // 障碍物 2：在参考线左侧 (Y=70 时参考线 X=14)，迫使车辆向右避让
        {START_UTM_X + 12.5, START_UTM_Y + 70.0, 2.5, 2.5},

        // 障碍物 3：在参考线右侧 (Y=100 时参考线 X=20)，再次迫使车辆向左避让
        {START_UTM_X + 21.0, START_UTM_Y + 100.0, 2.5, 2.5}
    };

    for (int frame = 0; frame < 500; ++frame) {
        auto real_state = simulator.GetState();
        
        Location sensor_loc;
        SimUTMToLatLon(real_state.x, real_state.y, sensor_loc.latitude, sensor_loc.longitude);
        sensor_loc.height = 0.0;
        sensor_loc.veloity = real_state.v;
        sensor_loc.acc = real_state.a;
        double heading_deg = 90.0 - (real_state.theta * 180.0 / M_PI);
        while (heading_deg < 0) heading_deg += 360.0;
        sensor_loc.heading = heading_deg;

        std::vector<GlobalPathPoint> interface_global_path;
        for (const auto& pt : sim_global_path_xy) {
            GlobalPathPoint gpt;
            SimUTMToLatLon(pt.first, pt.second, gpt.latitude, gpt.longitude);
            interface_global_path.push_back(gpt);
        }

        std::vector<double> occupancy_grid(1000 * 1000, 0.0);
        for (const auto& obs : sim_obstacles) {
            double rel_x = obs.x - real_state.x;
            double rel_y = obs.y - real_state.y;
            int center_c = 500 + static_cast<int>(rel_x / 0.2);
            int center_r = 500 + static_cast<int>(rel_y / 0.2);
            int half_w = static_cast<int>((obs.w / 2.0) / 0.2);
            int half_h = static_cast<int>((obs.h / 2.0) / 0.2);

            for (int r = center_r - half_h; r < center_r + half_h; ++r) {
                for (int c = center_c - half_w; c < center_c + half_w; ++c) {
                    if (r >= 0 && r < 1000 && c >= 0 && c < 1000) occupancy_grid[r * 1000 + c] = 1.0;
                }
            }
        }

        std::vector<PathPoint> output_trajectory;
        bool success = planner_interface.RunPlanner(sensor_loc, interface_global_path, occupancy_grid, 10.0, output_trajectory);

        double cmd_a = 0.0, cmd_kappa = 0.0;
        if (success && output_trajectory.size() > 1) {
            cmd_a = output_trajectory[1].a;         
            cmd_kappa = output_trajectory[1].kappa; 
        } else { cmd_a = -3.0; }
        simulator.Update(cmd_a, cmd_kappa, 0.1);

        // --- 渲染逻辑 ---
        cv::Mat display = cv::Mat::zeros(CANVAS_H, CANVAS_W + PANEL_W, CV_8UC3);
        cv::Mat canvas = display(cv::Rect(0, 0, CANVAS_W, CANVAS_H));
        cv::Mat panel = display(cv::Rect(CANVAS_W, 0, PANEL_W, CANVAS_H));

        cv::Point view_center(CANVAS_W / 2, CANVAS_H / 2);
        auto to_px = [&](double x, double y) {
            return cv::Point(view_center.x + (x - real_state.x) * SCALE, view_center.y - (y - real_state.y) * SCALE);
        };

        for (int i = 0; i <= CANVAS_W; i += static_cast<int>(10.0 * SCALE)) {
            int offset_x = i - static_cast<int>(std::fmod(real_state.x * SCALE, 10.0 * SCALE));
            int offset_y = i - static_cast<int>(std::fmod(real_state.y * SCALE, 10.0 * SCALE));
            if (offset_x >= 0 && offset_x <= CANVAS_W) cv::line(canvas, {offset_x, 0}, {offset_x, CANVAS_H}, {40, 40, 40}, 1);
            if (offset_y >= 0 && offset_y <= CANVAS_H) cv::line(canvas, {0, offset_y}, {CANVAS_W, offset_y}, {40, 40, 40}, 1);
        }

        for (size_t i = 1; i < sim_global_path_xy.size(); ++i) {
            cv::line(canvas, to_px(sim_global_path_xy[i-1].first, sim_global_path_xy[i-1].second), 
                             to_px(sim_global_path_xy[i].first, sim_global_path_xy[i].second), {100, 100, 100}, 2, cv::LINE_AA);
        }

        for (const auto& obs : sim_obstacles) {
            cv::Rect obs_rect(to_px(obs.x - obs.w/2, obs.y + obs.h/2), cv::Size(obs.w * SCALE, obs.h * SCALE));
            cv::rectangle(canvas, obs_rect, {0, 0, 255}, 2);
            cv::putText(canvas, "OBS", to_px(obs.x - obs.w/2, obs.y - obs.h/2 - 0.5), cv::FONT_HERSHEY_SIMPLEX, 0.5, {0, 0, 255}, 1);
        }

        if (success && output_trajectory.size() > 1) {
            for (size_t i = 1; i < output_trajectory.size(); ++i) {
                // 因为完美映射 UTM，输出轨迹直接等价于仿真坐标，再也不用加偏移量了！
                cv::line(canvas, to_px(output_trajectory[i-1].x, output_trajectory[i-1].y), 
                                 to_px(output_trajectory[i].x, output_trajectory[i].y), {0, 255, 0}, 2, cv::LINE_AA);

                // 核心修复：减去横向偏移 L，让黄色边界完美包络参考线
                double shift_left = output_trajectory[i].left_bound - output_trajectory[i].l;
                double shift_right = output_trajectory[i].right_bound - output_trajectory[i].l;
                double th = output_trajectory[i].theta;
                double nx = -std::sin(th), ny = std::cos(th);

                double l_x = output_trajectory[i].x + shift_left * nx;
                double l_y = output_trajectory[i].y + shift_left * ny;
                double r_x = output_trajectory[i].x + shift_right * nx;
                double r_y = output_trajectory[i].y + shift_right * ny;

                double shift_left_p = output_trajectory[i-1].left_bound - output_trajectory[i-1].l;
                double shift_right_p = output_trajectory[i-1].right_bound - output_trajectory[i-1].l;
                double th_p = output_trajectory[i-1].theta;
                double nx_p = -std::sin(th_p), ny_p = std::cos(th_p);

                double l_x_p = output_trajectory[i-1].x + shift_left_p * nx_p;
                double l_y_p = output_trajectory[i-1].y + shift_left_p * ny_p;
                double r_x_p = output_trajectory[i-1].x + shift_right_p * nx_p;
                double r_y_p = output_trajectory[i-1].y + shift_right_p * ny_p;

                cv::line(canvas, to_px(l_x_p, l_y_p), to_px(l_x, l_y), {0, 200, 255}, 1, cv::LINE_AA);
                cv::line(canvas, to_px(r_x_p, r_y_p), to_px(r_x, r_y), {0, 200, 255}, 1, cv::LINE_AA);
            }
        }

        DrawVehicleHUD(canvas, view_center, real_state.theta);

        // --- 看板逻辑 ---
        panel.setTo(cv::Scalar(25, 25, 25)); cv::line(panel, {0, 0}, {0, CANVAS_H}, {100, 100, 100}, 2); 
        int text_y = 50;
        auto draw_text = [&](const std::string& txt, cv::Scalar color = {255,255,255}, double scale = 0.7) {
            cv::putText(panel, txt, {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, scale, color, 1, cv::LINE_AA); text_y += 35;
        };

        draw_text("=== TITAN TELEMETRY ===", {0, 255, 255}, 0.8); text_y += 10;
        char buf[100];
        snprintf(buf, sizeof(buf), "Frame : %04d / %04d", frame, 500); draw_text(buf, {200, 200, 200});
        snprintf(buf, sizeof(buf), "Speed : %.2f km/h", real_state.v * 3.6); draw_text(buf, {0, 255, 0}, 0.8);
        snprintf(buf, sizeof(buf), "Accel : %.2f m/s^2", real_state.a); draw_text(buf);

        text_y += 20; draw_text("--- Sensor GPS ---", {0, 255, 255});
        snprintf(buf, sizeof(buf), "Lat : %.6f", sensor_loc.latitude); draw_text(buf); 
        snprintf(buf, sizeof(buf), "Lon : %.6f", sensor_loc.longitude); draw_text(buf);
        snprintf(buf, sizeof(buf), "Heading: %.2f deg", sensor_loc.heading); draw_text(buf);
        
        text_y += 20; draw_text("--- Local Physics ---", {0, 255, 255});
        snprintf(buf, sizeof(buf), "UTM X : %.2f m", real_state.x); draw_text(buf);
        snprintf(buf, sizeof(buf), "UTM Y : %.2f m", real_state.y); draw_text(buf);

        text_y += 50; draw_text("=== MAP LEGEND ===", {0, 255, 255}, 0.8); text_y += 10;
        cv::line(panel, {20, text_y-5}, {60, text_y-5}, {100, 100, 100}, 3); draw_text("   Global Ref Line", {200, 200, 200});
        cv::line(panel, {20, text_y-5}, {60, text_y-5}, {0, 255, 0}, 3);   draw_text("   Planned Trajectory", {0, 255, 0});
        cv::rectangle(panel, cv::Rect(20, text_y-15, 40, 15), {0, 0, 150}, -1); draw_text("   Obstacle Region", {0, 0, 255});
        cv::line(panel, {20, text_y-5}, {60, text_y-5}, {0, 200, 255}, 2); draw_text("   QP Virtual Bounds", {0, 255, 255});

        cv::imshow("Simulation Bridge", display);
        if (cv::waitKey(50) == 27) break;
    }

    return 0;
}