cmake_minimum_required(VERSION 3.31)
project(SolarTrackerMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# Find required packages
find_package(Threads REQUIRED)

add_executable(SolarTrackerMonitor
        main.cpp
        SerialPort.cpp
        SerialPort.h
        SolarData.h
        DataPlotter.cpp
        DataPlotter.h)

target_link_libraries(SolarTrackerMonitor PRIVATE Threads::Threads)
