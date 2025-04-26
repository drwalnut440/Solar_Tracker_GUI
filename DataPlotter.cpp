#include "DataPlotter.h"
#include <iostream>
#include <iomanip>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

DataPlotter::DataPlotter() : frameCount(0), quit(false) {}

DataPlotter::~DataPlotter() {}

void DataPlotter::clearScreen() {
#ifdef _WIN32
    // Windows clear screen
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
#else
    // ANSI escape codes for Unix-like systems
    std::cout << "\033[2J\033[1;1H";
#endif
}

// Đã bỏ định nghĩa hàm drawChart

void DataPlotter::displayData(const std::vector<SolarData>& data) {
    frameCount++;

    // Every 10 frames, check for input (non-blocking)
    if (frameCount % 10 == 0) {
        // Check for 'q' key press to quit
#ifdef _WIN32
        if (GetAsyncKeyState('Q') & 0x8000) {
            quit = true;
        }
#else
        // This is a simplified version. A proper implementation would use ncurses or similar
        // Giữ lại phần này nếu bạn muốn vẫn có thể nhấn 'q' để thoát trên Linux/macOS
        std::cout << "Press 'q' and Enter to quit: " << std::flush;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
            char c;
            if (read(STDIN_FILENO, &c, 1) > 0 && c == 'q') {
                quit = true;
            }
        }
#endif
    }

    clearScreen();

    // Display title and current time
    std::cout << "=== Solar Tracker Monitor ===" << std::endl;
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::cout << "Time: " << std::ctime(&now_c);
    std::cout << "Press 'q' to quit" << std::endl << std::endl;

    if (data.empty()) {
        std::cout << "Waiting for data..." << std::endl;
        return;
    }

    // Display latest values
    std::cout << "Current Values:" << std::endl;
    std::cout << "  Horizontal Angle: " << std::fixed << std::setprecision(2) << data.back().horizontalAngle << "°" << std::endl;
    std::cout << "  Vertical Angle: " << std::fixed << std::setprecision(2) << data.back().verticalAngle << "°" << std::endl;
    std::cout << "  Temperature: " << std::fixed << std::setprecision(2) << data.back().temperature << "°C" << std::endl << std::endl;

    // Đã bỏ các dòng gọi hàm drawChart
}

bool DataPlotter::shouldQuit() const {
    return quit;
}
