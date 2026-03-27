#ifndef WAYPOINT_DIRECT_RECORDER_HPP
#define WAYPOINT_DIRECT_RECORDER_HPP

#include <iomanip>
#include <optional>
#include <sstream>

#include "DirectRecorder.hpp"
#include "Waypoint.h"

namespace app_common {

    /**
     * @brief A RAII-based recorder for Waypoint structures using O_DIRECT
     *
     * This class records Waypoint structures to a file, one record per line.
     * Format: "lat,lon" (e.g., "31.2304567,121.4737123")
     *
     * Based on DirectRecorder for direct I/O operations.
     *
     * Note: Incomplete records (due to crash/power failure) are skipped during read.
     */
    class WaypointDirectRecorder {
    public:
        /**
         * @brief Construct a WaypointDirectRecorder and open the file
         *
         * @param filepath Path to the file to open
         * @param mode File open mode (CREATE_TRUNC_APPEND or APPEND_ONLY)
         * @throws std::runtime_error if file cannot be opened
         */
        explicit WaypointDirectRecorder(const std::string& filepath,
                                        DirectRecorderMode mode = DirectRecorderMode::CREATE_TRUNC_APPEND)
            : _recorder(filepath, mode) {
        }

        /**
         * @brief Destructor - closes the file automatically (RAII)
         */
        ~WaypointDirectRecorder() = default;

        // Disable copy and move (DirectRecorder is not movable)
        WaypointDirectRecorder(const WaypointDirectRecorder&) = delete;

        WaypointDirectRecorder& operator=(const WaypointDirectRecorder&) = delete;

        WaypointDirectRecorder(WaypointDirectRecorder&&) = delete;

        WaypointDirectRecorder& operator=(WaypointDirectRecorder&&) = delete;

        /**
         * @brief Append a Waypoint record to the file
         *
         * @param latlon The Waypoint structure to append
         * @throws std::runtime_error if write fails
         */
        void append(const Waypoint& latlon) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(7) << latlon.lat << "," << latlon.lon;
            _recorder.append(oss.str());
        }

        void append(double lat, double lon) {
            Waypoint ll(lat, lon);
            append(ll);
        }

        /**
         * @brief Read all valid Waypoint records from the file
         *
         * Incomplete or malformed records are skipped.
         *
         * @return std::vector<Waypoint> Vector of all valid Waypoint records
         * @throws std::runtime_error if file cannot be read
         */
        std::vector<Waypoint> read_all() const {
            std::vector<Waypoint> records;
            auto                  lines = _recorder.read_all();

            for (const auto& line : lines) {
                auto latlon = parse_line(line);
                if (latlon.has_value()) {
                    records.push_back(latlon.value());
                }
            }

            return records;
        }

    private:
        /**
         * @brief Parse a line into Waypoint structure
         *
         * @param line The line to parse
         * @return std::optional<Waypoint> The parsed Waypoint, or empty if invalid
         */
        static std::optional<Waypoint> parse_line(const std::string& line) {
            if (line.empty()) {
                return std::nullopt;
            }

            size_t comma_pos = line.find(',');
            if (comma_pos == std::string::npos) {
                return std::nullopt;
            }

            try {
                Waypoint latlon;
                latlon.lat = std::stod(line.substr(0, comma_pos));
                latlon.lon = std::stod(line.substr(comma_pos + 1));
                return latlon;
            } catch (const std::invalid_argument&) {
                return std::nullopt; // Invalid number format
            } catch (const std::out_of_range&) {
                return std::nullopt; // Number out of range
            }
        }

    private:
        DirectRecorder _recorder;
    };

} // namespace app_common

#endif // LAT_LON_DIRECT_RECORDER_HPP
