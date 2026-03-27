#include "utm.h"

// 辅助函数实现
bool in_bounds(double value, double lower, double upper, bool upper_strict) {
    if (upper_strict) {
        return lower <= value && value < upper;
    } else {
        return lower <= value && value <= upper;
    }
}

void check_valid_zone_letter(const std::string& zone_letter) {
    if (zone_letter.empty()) return;
    
    char letter = toupper(zone_letter[0]);
    if (!(('C' <= letter && letter <= 'X') && letter != 'I' && letter != 'O')) {
        throw OutOfRangeError("zone letter out of range (must be between C and X, excluding I and O)");
    }
}

void check_valid_zone_number(int zone_number) {
    if (!(1 <= zone_number && zone_number <= 60)) {
        throw OutOfRangeError("zone number out of range (must be between 1 and 60)");
    }
}

void check_valid_zone(int zone_number, const std::string& zone_letter) {
    check_valid_zone_number(zone_number);
    if (!zone_letter.empty()) {
        check_valid_zone_letter(zone_letter);
    }
}

bool mixed_signs(double latitude) {
    // 对于单个值，始终返回false
    return false;
}

double mod_angle(double value) {
    return fmod(value + M_PI, 2 * M_PI) - M_PI;
}

std::string latitude_to_zone_letter(double latitude) {
    if (-80 <= latitude && latitude <= 84) {
        int index = static_cast<int>(latitude + 80) >> 3;
        if (index >= 0 && index < ZONE_LETTERS.length()) {
            return std::string(1, ZONE_LETTERS[index]);
        }
    }
    return "";
}

int latlon_to_zone_number(double latitude, double longitude) {
    // 归一化经度到 [-180, 180)
    longitude = fmod(longitude + 180, 360);
    if (longitude < 0) longitude += 360;
    longitude -= 180;
    
    // 挪威特殊区域
    if (56 <= latitude && latitude < 64 && 3 <= longitude && longitude < 12) {
        return 32;
    }
    
    // 斯瓦尔巴德群岛特殊区域
    if (72 <= latitude && latitude <= 84 && longitude >= 0) {
        if (longitude < 9) return 31;
        else if (longitude < 21) return 33;
        else if (longitude < 33) return 35;
        else if (longitude < 42) return 37;
    }
    
    return static_cast<int>((longitude + 180) / 6) + 1;
}

double zone_number_to_central_longitude(int zone_number) {
    check_valid_zone_number(zone_number);
    return (zone_number - 1) * 6 - 180 + 3;
}

double zone_letter_to_central_latitude(const std::string& zone_letter) {
    check_valid_zone_letter(zone_letter);
    char letter = toupper(zone_letter[0]);
    
    if (letter == 'X') {
        return 78;
    } else {
        size_t index = ZONE_LETTERS.find(letter);
        if (index != std::string::npos) {
            return -76 + (index * 8);
        }
    }
    return 0;
}

// UTM转经纬度
LatLonResult to_latlon(double easting, double northing, int zone_number, 
                       const std::string& zone_letter, bool northern, 
                       bool strict) {
    
    if (zone_letter.empty() && !northern) {
        throw std::invalid_argument("either zone_letter or northern needs to be set");
    }
    else if (!zone_letter.empty() && northern) {
        throw std::invalid_argument("set either zone_letter or northern, but not both");
    }
    
    if (strict) {
        if (!in_bounds(easting, 100000, 1000000, true)) {
            throw OutOfRangeError("easting out of range (must be between 100,000 m and 999,999 m)");
        }
        if (!in_bounds(northing, 0, 10000000)) {
            throw OutOfRangeError("northing out of range (must be between 0 m and 10,000,000 m)");
        }
    }
    
    check_valid_zone(zone_number, zone_letter);
    
    if (!zone_letter.empty()) {
        northern = (toupper(zone_letter[0]) >= 'N');
    }
    
    double x = easting - 500000;
    double y = northern ? northing : northing - 10000000;
    
    double m = y / K0;
    double mu = m / (R * M1);
    
    double p_rad = mu +
                   P2 * sin(2 * mu) +
                   P3 * sin(4 * mu) +
                   P4 * sin(6 * mu) +
                   P5 * sin(8 * mu);
    
    double p_sin = sin(p_rad);
    double p_sin2 = p_sin * p_sin;
    double p_cos = cos(p_rad);
    double p_tan = p_sin / p_cos;
    double p_tan2 = p_tan * p_tan;
    double p_tan4 = p_tan2 * p_tan2;
    
    double ep_sin = 1 - E * p_sin2;
    double ep_sin_sqrt = sqrt(ep_sin);
    
    double n = R / ep_sin_sqrt;
    double r = (1 - E) / ep_sin;
    
    double c = E_P2 * p_cos * p_cos;
    double c2 = c * c;
    
    double d = x / (n * K0);
    double d2 = d * d;
    double d3 = d2 * d;
    double d4 = d3 * d;
    double d5 = d4 * d;
    double d6 = d5 * d;
    
    double latitude = p_rad - (p_tan / r) * (
                     d2 / 2 -
                     d4 / 24 * (5 + 3 * p_tan2 + 10 * c - 4 * c2 - 9 * E_P2) +
                     d6 / 720 * (61 + 90 * p_tan2 + 298 * c + 45 * p_tan4 - 252 * E_P2 - 3 * c2));
    
    double longitude = (d -
                       d3 / 6 * (1 + 2 * p_tan2 + c) +
                       d5 / 120 * (5 - 2 * c + 28 * p_tan2 - 3 * c2 + 8 * E_P2 + 24 * p_tan4)) / p_cos;
    
    double central_lon_rad = M_PI * zone_number_to_central_longitude(zone_number) / 180.0;
    longitude = mod_angle(longitude + central_lon_rad);
    
    return {latitude * 180.0 / M_PI, longitude * 180.0 / M_PI};
}

// 经纬度转UTM
UTMResult from_latlon(double latitude, double longitude, 
                      int force_zone_number, 
                      const std::string& force_zone_letter,
                      bool* force_northern) {
    
    if (!in_bounds(latitude, -80.0, 84.0)) {
        throw OutOfRangeError("latitude out of range (must be between 80 deg S and 84 deg N)");
    }
    if (!in_bounds(longitude, -180.0, 180.0)) {
        throw OutOfRangeError("longitude out of range (must be between 180 deg W and 180 deg E)");
    }
    if (!force_zone_letter.empty() && force_northern != nullptr) {
        throw std::invalid_argument("set either force_zone_letter or force_northern, but not both");
    }
    if (force_zone_number != -1) {
        check_valid_zone(force_zone_number, force_zone_letter);
    }
    
    double lat_rad = M_PI * latitude / 180.0;
    double lat_sin = sin(lat_rad);
    double lat_cos = cos(lat_rad);
    double lat_tan = lat_sin / lat_cos;
    double lat_tan2 = lat_tan * lat_tan;
    double lat_tan4 = lat_tan2 * lat_tan2;
    
    int zone_number = (force_zone_number != -1) ? force_zone_number : 
                      latlon_to_zone_number(latitude, longitude);
    
    std::string zone_letter;
    if (force_zone_letter.empty() && force_northern == nullptr) {
        zone_letter = latitude_to_zone_letter(latitude);
    } else {
        zone_letter = force_zone_letter;
    }
    
    bool northern;
    if (force_northern == nullptr) {
        northern = (!zone_letter.empty() && zone_letter >= "N");
    } else {
        northern = *force_northern;
    }
    
    double lon_rad = M_PI * longitude / 180.0;
    double central_lon = zone_number_to_central_longitude(zone_number);
    double central_lon_rad = M_PI * central_lon / 180.0;
    
    double n = R / sqrt(1 - E * lat_sin * lat_sin);
    double c = E_P2 * lat_cos * lat_cos;
    
    double a = lat_cos * mod_angle(lon_rad - central_lon_rad);
    double a2 = a * a;
    double a3 = a2 * a;
    double a4 = a3 * a;
    double a5 = a4 * a;
    double a6 = a5 * a;
    
    double m = R * (M1 * lat_rad -
                   M2 * sin(2 * lat_rad) +
                   M3 * sin(4 * lat_rad) -
                   M4 * sin(6 * lat_rad));
    
    double easting = K0 * n * (a +
                              a3 / 6 * (1 - lat_tan2 + c) +
                              a5 / 120 * (5 - 18 * lat_tan2 + lat_tan4 + 72 * c - 58 * E_P2)) + 500000;
    
    double northing = K0 * (m + n * lat_tan * (a2 / 2 +
                                              a4 / 24 * (5 - lat_tan2 + 9 * c + 4 * c*c) +
                                              a6 / 720 * (61 - 58 * lat_tan2 + lat_tan4 + 600 * c - 330 * E_P2)));
    
    bool check_signs = (force_northern == nullptr && force_zone_letter.empty());
    if (check_signs && mixed_signs(latitude)) {
        throw std::invalid_argument("latitudes must all have the same sign");
    } else if (!northern) {
        northing += 10000000;
    }
    
    return {easting, northing, zone_number, zone_letter};
}
