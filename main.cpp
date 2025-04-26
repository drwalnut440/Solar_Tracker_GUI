#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <atomic>
#include "SerialPort.h"
#include "SolarData.h"
#include "DataPlotter.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::mutex dataMutex;
std::vector<SolarData> dataPoints;
std::atomic<bool> running(true);

void dataCollectionThread(const std::string& portName) {
    SerialPort serialPort(portName);

    if (!serialPort.isConnected()) {
        std::cerr << "Failed to connect to port " << portName << std::endl;
        running = false;
        return;
    }

    std::cout << "Connected to Arduino on port " << portName << std::endl;

    while (running) {
        std::string data = serialPort.readLine();

        if (!data.empty()) {
            try {
                // Parse CSV data: horizontal,vertical,temperature
                size_t pos1 = data.find(',');
                size_t pos2 = data.find(',', pos1 + 1);

                if (pos1 != std::string::npos && pos2 != std::string::npos) {
                    float horizontal = std::stof(data.substr(0, pos1));
                    float vertical = std::stof(data.substr(pos1 + 1, pos2 - pos1 - 1));
                    float temp = std::stof(data.substr(pos2 + 1));

                    SolarData point;
                    point.timestamp = std::chrono::system_clock::now();
                    point.horizontalAngle = horizontal;
                    point.verticalAngle = vertical;
                    point.temperature = temp;

                    std::lock_guard<std::mutex> lock(dataMutex);
                    dataPoints.push_back(point);

                    // Keep only the last 100 data points
                    if (dataPoints.size() > 100) {
                        dataPoints.erase(dataPoints.begin());
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing data: " << e.what() << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
#ifdef _WIN32
    // Thiết lập Output Code Page thành UTF-8 trên Windows để hiển thị đúng ký tự đặc biệt
    SetConsoleOutputCP(CP_UTF8);
    // Tùy chọn: Thiết lập Input Code Page thành UTF-8 nếu bạn cần nhập ký tự UTF-8
    // SetConsoleCP(CP_UTF8);
#endif

    std::string portName;

    std::cout << "Solar Tracker Monitor" << std::endl;
    std::cout << "Enter Arduino serial port (e.g., COM3 on Windows, /dev/ttyACM0 on Linux): ";
    std::cin >> portName;

    // Start data collection thread
    std::thread collector(dataCollectionThread, portName);

    // Create data plotter
    DataPlotter plotter;

    // Main display loop
    while (running) {
        std::vector<SolarData> currentData;
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            currentData = dataPoints;
        }

        plotter.displayData(currentData);

        // Check for quit command
        if (plotter.shouldQuit()) {
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Wait for collector thread to finish
    collector.join();

    std::cout << "Program terminated." << std::endl;
    return 0;
}
