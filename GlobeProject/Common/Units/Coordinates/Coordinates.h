#pragma once
#ifndef COORDINATES_H
#define COORDINATES_H

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// WGS84 ellipsoid constants
const double WGS84_A = 6378137.0;                // Semi-major axis (meters)
const double WGS84_F = 1.0 / 298.257223563;      // Flattening
const double WGS84_B = WGS84_A * (1.0 - WGS84_F); // Semi-minor axis (meters)
const double WGS84_E2 = WGS84_F * (2.0 - WGS84_F); // Eccentricity squared

class ECEF;
class LLA;

class LLA {
public:
    double latitude;  // in degrees
    double longitude; // in degrees
    double altitude;  // in meters

    LLA(double lat, double lon, double alt) : latitude(lat), longitude(lon), altitude(alt) {}
};

class ECEF {
public:
    double x; // in meters
    double y; // in meters
    double z; // in meters

    ECEF(double x, double y, double z) : x(x), y(y), z(z) {}
};

class CoordinateConverter {
public:
    // Convert degrees to radians
    static double deg2rad(double degrees) {
        return degrees * M_PI / 180.0;
    }

    // Convert radians to degrees
    static double rad2deg(double radians) {
        return radians * 180.0 / M_PI;
    }

    // Convert LLA to ECEF
    static ECEF toECEF(const LLA& lla) {
        double latRad = deg2rad(lla.latitude);
        double lonRad = deg2rad(lla.longitude);

        double N = WGS84_A / std::sqrt(1.0 - WGS84_E2 * std::sin(latRad) * std::sin(latRad));
        double x = (N + lla.altitude) * std::cos(latRad) * std::cos(lonRad);
        double y = (N + lla.altitude) * std::cos(latRad) * std::sin(lonRad);
        double z = ((1.0 - WGS84_E2) * N + lla.altitude) * std::sin(latRad);

        return ECEF(x, y, z);
    }

    // Convert ECEF to LLA
    static LLA toLLA(const ECEF& ecef) {
        double p = std::sqrt(ecef.x * ecef.x + ecef.y * ecef.y);
        double theta = std::atan2(ecef.z * WGS84_A, p * WGS84_B);
        double lon = std::atan2(ecef.y, ecef.x);
        double lat = std::atan2(ecef.z + WGS84_E2 * WGS84_B * std::sin(theta) * std::sin(theta) * std::sin(theta),
            p - WGS84_E2 * WGS84_A * std::cos(theta) * std::cos(theta) * std::cos(theta));
        double N = WGS84_A / std::sqrt(1.0 - WGS84_E2 * std::sin(lat) * std::sin(lat));
        double alt = p / std::cos(lat) - N;

        return LLA(rad2deg(lat), rad2deg(lon), alt);
    }
};

#endif // COORDINATES_H
