#pragma once

namespace app_common {
    struct LatLon {
        double lat; // latitude
        double lon; // longitude

        LatLon(double lat_, double lon_) : lat(lat_), lon(lon_) {
        }

        LatLon() : LatLon(0.0, 0.0) {
        }
    };

} // namespace app_common