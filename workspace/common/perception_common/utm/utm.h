#ifndef UTM_H
#define UTM_H

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <iomanip>

// 常量定义
const double K0 = 0.9996;
const double E = 0.00669438;
const double E2 = E * E;
const double E3 = E2 * E;
const double E_P2 = E / (1 - E);
const double SQRT_E = sqrt(1 - E);
const double _E = (1 - SQRT_E) / (1 + SQRT_E);
const double _E2 = _E * _E;
const double _E3 = _E2 * _E;
const double _E4 = _E3 * _E;
const double _E5 = _E4 * _E;

const double M1 = (1 - E / 4 - 3 * E2 / 64 - 5 * E3 / 256);
const double M2 = (3 * E / 8 + 3 * E2 / 32 + 45 * E3 / 1024);
const double M3 = (15 * E2 / 256 + 45 * E3 / 1024);
const double M4 = (35 * E3 / 3072);

const double P2 = (3 / 2 * _E - 27 / 32 * _E3 + 269 / 512 * _E5);
const double P3 = (21 / 16 * _E2 - 55 / 32 * _E4);
const double P4 = (151 / 96 * _E3 - 417 / 128 * _E5);
const double P5 = (1097 / 512 * _E4);

const double R = 6378137;
const std::string ZONE_LETTERS = "CDEFGHJKLMNPQRSTUVWXX";

// 异常类定义
class OutOfRangeError : public std::runtime_error {
public:
    OutOfRangeError(const std::string& message) : std::runtime_error(message) {}
};

// 结果结构体
struct LatLonResult {
    double latitude;
    double longitude;
};

struct UTMResult {
    double easting;
    double northing;
    int zone_number;
    std::string zone_letter;
};

// 辅助函数声明
bool in_bounds(double value, double lower, double upper, bool upper_strict = false);
void check_valid_zone_letter(const std::string& zone_letter);
void check_valid_zone_number(int zone_number);
void check_valid_zone(int zone_number, const std::string& zone_letter);
bool mixed_signs(double latitude);
double mod_angle(double value);
std::string latitude_to_zone_letter(double latitude);
int latlon_to_zone_number(double latitude, double longitude);
double zone_number_to_central_longitude(int zone_number);
double zone_letter_to_central_latitude(const std::string& zone_letter);

LatLonResult to_latlon(double easting, double northing, int zone_number, 
                       const std::string& zone_letter = "", bool northern = false, 
                       bool strict = true);
                       
// 经纬度转UTM
UTMResult from_latlon(double latitude, double longitude, 
                      int force_zone_number = -1, 
                      const std::string& force_zone_letter = "",
                      bool* force_northern = nullptr);
                      
#endif
