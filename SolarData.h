#ifndef SOLAR_TRACKER_SOLARDATA_H
#define SOLAR_TRACKER_SOLARDATA_H

#include <chrono>

struct SolarData {
    std::chrono::system_clock::time_point timestamp;
    float horizontalAngle;
    float verticalAngle;
    float temperature;
};

#endif // SOLAR_TRACKER_SOLARDATA_H