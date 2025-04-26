#ifndef SOLAR_TRACKER_DATAPLOTTER_H
#define SOLAR_TRACKER_DATAPLOTTER_H

#include <vector>
#include <iostream>
#include "SolarData.h"

class DataPlotter {
public:
    DataPlotter();
    ~DataPlotter();

    void displayData(const std::vector<SolarData>& data);
    bool shouldQuit() const;

private:
    int frameCount;
    bool quit;

    void clearScreen();
    // Đã bỏ khai báo hàm drawChart
};

#endif // SOLAR_TRACKER_DATAPLOTTER_H