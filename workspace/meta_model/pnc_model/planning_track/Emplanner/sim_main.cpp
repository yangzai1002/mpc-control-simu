#include <iostream>
#include <vector>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <cmath>

#include "planner_interface.h"
#include "simulation/vehicle_simulator.h"

using namespace localplanner;
using namespace localplanner::interface;

void SimUTMToLatLon(double x, double y, double& lat, double& lon) {
    double a = 6378137.0;
    double f = 1.0 / 298.257223563;
    double b = a * (1 - f);
    double e = std::sqrt(1 - (b * b) / (a * a));
    double e2 = e * e;
    double e_prime_2 = e2 / (1 - e2);

    int zone = 51; 
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
                     - (5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * e_prime_2) * std::pow(D, 4) / 24
                     + (61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * e_prime_2 - 3 * C1 * C1) * std::pow(D, 6) / 720);

    double lon_0 = (zone - 1) * 6 - 180 + 3;
    double lon_rad = lon_0 * M_PI / 180.0 + (D - (1 + 2 * T1 + C1) * std::pow(D, 3) / 6
                     + (5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * e_prime_2 + 24 * T1 * T1) * std::pow(D, 5) / 120) / std::cos(phi1);

    lat = lat_rad * 180.0 / M_PI;
    lon = lon_rad * 180.0 / M_PI;
}

const int CANVAS_W = 1000, CANVAS_H = 1000, PANEL_W = 450;

void DrawVehicleHUD(cv::Mat& img, const cv::Point& center, double heading_rad, double scale) {
    double l = 4.8 * scale, w = 2.0 * scale; 
    cv::RotatedRect car_rect(center, cv::Size2f(l, w), -heading_rad * 180.0 / M_PI);
    cv::Point2f pts[4];
    car_rect.points(pts);
    
    std::vector<cv::Point> hull;
    for(int i=0; i<4; i++) hull.push_back(pts[i]);
    cv::fillConvexPoly(img, hull, cv::Scalar(150, 70, 50)); 
    for (int i = 0; i < 4; i++) cv::line(img, pts[i], pts[(i + 1) % 4], {255, 255, 255}, 2, cv::LINE_AA);
    
    cv::Point front_windshield = center + cv::Point(std::cos(heading_rad) * l * 0.25, -std::sin(heading_rad) * l * 0.25);
    cv::circle(img, front_windshield, static_cast<int>(w * 0.3), {255, 200, 100}, -1, cv::LINE_AA);

    cv::Point arrow_tip = center + cv::Point(std::cos(heading_rad) * l * 0.8, -std::sin(heading_rad) * l * 0.8);
    cv::arrowedLine(img, center, arrow_tip, {0, 255, 255}, 2, 8, 0, 0.2);
}

struct SimObs { double x, y, w, h; };

int main() {
    std::cout << "[INFO] Simulation Bridge Booting..." << std::endl;
    PlannerInterface planner_interface;

    const double START_UTM_X = 500000.0;
    const double START_UTM_Y = 3450000.0;

    simulation::VehicleSimulator::Config sim_cfg;
    simulation::VehicleSimulator::State sim_state;
    sim_state.x = START_UTM_X;
    sim_state.y = START_UTM_Y;
    sim_state.theta = std::atan2(10.0, 2.0); 
    sim_state.v = 0.0;            
    simulation::VehicleSimulator simulator(sim_cfg, sim_state);

    std::vector<std::pair<double, double>> sim_global_path_xy;
    for (int i = 0; i < 40; ++i) {
        sim_global_path_xy.push_back({START_UTM_X + i * 2.0, START_UTM_Y + i * 10.0});
    }

    std::vector<SimObs> sim_obstacles = {
        {START_UTM_X + 8.0, START_UTM_Y + 40.0, 2.5, 2.5}, 
        {START_UTM_X + 13.5, START_UTM_Y + 65.0, 2.5, 2.5},
        {START_UTM_X + 19.0, START_UTM_Y + 95.0, 2.5, 2.5}
    };

    // 窗口可改变大小
    cv::namedWindow("Simulation Bridge", cv::WINDOW_NORMAL);
    cv::resizeWindow("Simulation Bridge", CANVAS_W + PANEL_W, CANVAS_H);

    double current_scale = 10.0; // 初始缩放

    for (int frame = 0; frame < 600; ++frame) {
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
        bool success = planner_interface.RunPlanner(sensor_loc, interface_global_path, occupancy_grid, 15.0, output_trajectory);

        double cmd_a = 0.0, cmd_kappa = 0.0;
        if (success && output_trajectory.size() > 1) {
            cmd_a = output_trajectory[1].a;         
            cmd_kappa = output_trajectory[1].kappa; 
        } else { 
            cmd_a = -3.0; 
        }
        simulator.Update(cmd_a, cmd_kappa, 0.1);

        double min_obs_dist = 999.0;
        for(const auto& obs : sim_obstacles) {
            double dist = std::hypot(obs.x - real_state.x, obs.y - real_state.y);
            if(dist < min_obs_dist) min_obs_dist = dist;
        }

        cv::Mat display = cv::Mat::zeros(CANVAS_H, CANVAS_W + PANEL_W, CV_8UC3);
        cv::Mat canvas = display(cv::Rect(0, 0, CANVAS_W, CANVAS_H));
        cv::Mat panel = display(cv::Rect(CANVAS_W, 0, PANEL_W, CANVAS_H));

        // 视角锁定正中
        cv::Point view_center(CANVAS_W / 2, CANVAS_H / 2); 
        auto to_px = [&](double x, double y) {
            return cv::Point(view_center.x + (x - real_state.x) * current_scale, view_center.y - (y - real_state.y) * current_scale);
        };

        for (int i = 0; i <= CANVAS_W; i += static_cast<int>(10.0 * current_scale)) {
            int offset_x = i - static_cast<int>(std::fmod(real_state.x * current_scale, 10.0 * current_scale));
            int offset_y = i - static_cast<int>(std::fmod(real_state.y * current_scale, 10.0 * current_scale));
            if (offset_x >= 0 && offset_x <= CANVAS_W) cv::line(canvas, {offset_x, 0}, {offset_x, CANVAS_H}, {30, 30, 30}, 1);
            if (offset_y >= 0 && offset_y <= CANVAS_H) cv::line(canvas, {0, offset_y}, {CANVAS_W, offset_y}, {30, 30, 30}, 1);
        }

        for (size_t i = 1; i < sim_global_path_xy.size(); ++i) {
            cv::line(canvas, to_px(sim_global_path_xy[i-1].first, sim_global_path_xy[i-1].second), 
                             to_px(sim_global_path_xy[i].first, sim_global_path_xy[i].second), {80, 80, 80}, 3, cv::LINE_AA);
        }

        for (const auto& obs : sim_obstacles) {
            cv::Rect obs_rect(to_px(obs.x - obs.w/2, obs.y + obs.h/2), cv::Size(obs.w * current_scale, obs.h * current_scale));
            cv::Rect shadow_rect = obs_rect; shadow_rect.x += 3; shadow_rect.y += 3;
            cv::rectangle(canvas, shadow_rect, {20, 20, 20}, -1);
            cv::rectangle(canvas, obs_rect, {50, 50, 200}, -1);
            cv::rectangle(canvas, obs_rect, {100, 100, 255}, 2);
            cv::putText(canvas, "STATIC", to_px(obs.x - obs.w/2, obs.y - obs.h/2 - 0.5), cv::FONT_HERSHEY_SIMPLEX, 0.4, {200, 200, 255}, 1, cv::LINE_AA);
        }

        double max_kappa = 0.0;
        if (success && output_trajectory.size() > 1) {
            for (size_t i = 1; i < output_trajectory.size(); ++i) {
                if (std::abs(output_trajectory[i].kappa) > max_kappa) max_kappa = std::abs(output_trajectory[i].kappa);

                double shift_left = output_trajectory[i].left_bound - output_trajectory[i].l;
                double shift_right = output_trajectory[i].right_bound - output_trajectory[i].l;
                double th = output_trajectory[i].theta;
                double nx = -std::sin(th), ny = std::cos(th);

                double l_x = output_trajectory[i].x + shift_left * nx, l_y = output_trajectory[i].y + shift_left * ny;
                double r_x = output_trajectory[i].x + shift_right * nx, r_y = output_trajectory[i].y + shift_right * ny;

                double shift_left_p = output_trajectory[i-1].left_bound - output_trajectory[i-1].l;
                double shift_right_p = output_trajectory[i-1].right_bound - output_trajectory[i-1].l;
                double th_p = output_trajectory[i-1].theta;
                double nx_p = -std::sin(th_p), ny_p = std::cos(th_p);

                double l_x_p = output_trajectory[i-1].x + shift_left_p * nx_p, l_y_p = output_trajectory[i-1].y + shift_left_p * ny_p;
                double r_x_p = output_trajectory[i-1].x + shift_right_p * nx_p, r_y_p = output_trajectory[i-1].y + shift_right_p * ny_p;

                cv::line(canvas, to_px(l_x_p, l_y_p), to_px(l_x, l_y), {0, 150, 200}, 1, cv::LINE_AA);
                cv::line(canvas, to_px(r_x_p, r_y_p), to_px(r_x, r_y), {0, 150, 200}, 1, cv::LINE_AA);
                cv::line(canvas, to_px(output_trajectory[i-1].x, output_trajectory[i-1].y), 
                                 to_px(output_trajectory[i].x, output_trajectory[i].y), {0, 255, 0}, 2, cv::LINE_AA);
                cv::circle(canvas, to_px(output_trajectory[i].x, output_trajectory[i].y), 2, {255, 255, 255}, -1, cv::LINE_AA);
            }
        }

        DrawVehicleHUD(canvas, view_center, real_state.theta, current_scale);

        panel.setTo(cv::Scalar(15, 15, 15)); 
        cv::line(panel, {0, 0}, {0, CANVAS_H}, {80, 80, 80}, 2); 
        
        int text_y = 30;
        char buf[100];

        auto draw_section_title = [&](const std::string& title) {
            text_y += 10;
            cv::rectangle(panel, cv::Rect(10, text_y - 20, PANEL_W - 20, 30), {45, 45, 45}, -1);
            cv::putText(panel, title, {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {255, 200, 50}, 2, cv::LINE_AA);
            text_y += 30;
        };

        auto draw_kv = [&](const std::string& key, const std::string& val, cv::Scalar val_color = {220, 220, 220}) {
            cv::putText(panel, key, {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.55, {150, 150, 150}, 1, cv::LINE_AA);
            cv::putText(panel, val, {200, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.55, val_color, 1, cv::LINE_AA);
            text_y += 25;
        };

        cv::putText(panel, "SIMULATION BRIDGE CONSOLE", {30, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.7, {255, 255, 255}, 2, cv::LINE_AA);
        text_y += 20;

        draw_section_title("[ EGO KINEMATICS ]");
        snprintf(buf, sizeof(buf), "%.2f km/h", real_state.v * 3.6); draw_kv("Current Speed:", buf, {0, 255, 0});
        snprintf(buf, sizeof(buf), "%.2f m/s^2", real_state.a);      draw_kv("Acceleration:", buf);
        snprintf(buf, sizeof(buf), "%.1f deg", sensor_loc.heading);  draw_kv("Math Heading:", buf);
        snprintf(buf, sizeof(buf), "%.1f, %.1f", real_state.x, real_state.y); draw_kv("UTM (X,Y):", buf);

        draw_section_title("[ FRENET STATE ]");
        if(success && !output_trajectory.empty()) {
            snprintf(buf, sizeof(buf), "%.2f m", output_trajectory[0].l); draw_kv("Lateral Error (L):", buf, 
                std::abs(output_trajectory[0].l) > 1.0 ? cv::Scalar(0, 150, 255) : cv::Scalar(255, 255, 255)); 
        } else {
            draw_kv("Lateral Error (L):", "N/A");
        }

        draw_section_title("[ ENVIRONMENT ]");
        snprintf(buf, sizeof(buf), "%zu detected", sim_obstacles.size()); draw_kv("Static Obstacles:", buf);
        snprintf(buf, sizeof(buf), "%.1f m", min_obs_dist); draw_kv("Nearest Obs Dist:", buf, 
            min_obs_dist < 5.0 ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 255)); 

        draw_section_title("[ PLANNING METRICS ]");
        draw_kv("Solve Status:", success ? "SUCCESS" : "FALLBACK / AEB", success ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255));
        if (success && output_trajectory.size() > 1) {
            snprintf(buf, sizeof(buf), "%zu nodes", output_trajectory.size()); draw_kv("Trajectory Length:", buf);
            
            double end_v = output_trajectory.back().v;
            snprintf(buf, sizeof(buf), "%.2f km/h", end_v * 3.6); draw_kv("Target End Speed:", buf,
                end_v < 0.1 ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 255, 255)); 
                
            snprintf(buf, sizeof(buf), "%.4f 1/m", max_kappa); draw_kv("Max Curvature:", buf);
            
            snprintf(buf, sizeof(buf), "%.2f m/s^2", output_trajectory[1].a); 
            draw_kv("Cmd Acceleration:", buf, output_trajectory[1].a < 0 ? cv::Scalar(0, 150, 255) : cv::Scalar(0, 255, 0));
        }

        // 显示交互快捷键提示
        text_y = CANVAS_H - 140;
        cv::rectangle(panel, cv::Rect(10, text_y, PANEL_W - 20, 130), {30, 30, 30}, -1);
        text_y += 25;
        cv::putText(panel, "=== INTERACTIVE CONTROLS ===", {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.6, {0, 255, 255}, 1, cv::LINE_AA); text_y += 25;
        cv::putText(panel, "[ + ] / [ = ] : Zoom IN (Scale Up)", {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {200, 200, 200}, 1, cv::LINE_AA); text_y += 25;
        cv::putText(panel, "[ - ] / [ _ ] : Zoom OUT (Scale Down)", {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {200, 200, 200}, 1, cv::LINE_AA); text_y += 25;
        cv::putText(panel, "[ ESC ]       : Exit Simulation", {20, text_y}, cv::FONT_HERSHEY_SIMPLEX, 0.5, {0, 0, 255}, 1, cv::LINE_AA);

        cv::imshow("Simulation Bridge", display);
        
        int key = cv::waitKey(50);
        if (key == 27) break; 
        if (key == '=' || key == '+') current_scale *= 1.1; 
        if (key == '-' || key == '_') current_scale /= 1.1; 
    }

    return 0;
}