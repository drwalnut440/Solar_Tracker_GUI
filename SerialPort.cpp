#include "SerialPort.h"
#include <iostream>
#include <locale>   // Thêm để thiết lập locale
#include <codecvt>  // Thêm để chuyển đổi giữa các mã hóa

#ifdef _WIN32
#include <Windows.h>
#endif

class SerialPortImpl {
public:
    explicit SerialPortImpl(const std::string& portName, int baudRate) : baudRate_(baudRate) {
        connected = false;

#ifdef _WIN32
        // Windows implementation
        std::string fullPortName = "\\\\.\\" + portName;
        // Sử dụng wcout để in ra console với UTF-8
        std::wcout.imbue(std::locale("")); // Thiết lập locale của wcout
        std::wcout << L"Đang cố gắng mở cổng COM: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(portName)
                  << L" với tốc độ baud: " << baudRate << std::endl;
        hSerial = CreateFileA(fullPortName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hSerial == INVALID_HANDLE_VALUE) {
            // Sử dụng wcerr để in lỗi với UTF-8
            std::wcerr.imbue(std::locale(""));
            std::wcerr << L"Lỗi khi mở cổng COM " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(portName) << L": " << GetLastError() << std::endl;
            std::wcerr << L"Vui lòng kiểm tra xem cổng COM có đúng không và không có ứng dụng nào khác đang sử dụng nó." << std::endl;
            return;
        }

        DCB dcbSerialParams = { 0 };
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        if (!GetCommState(hSerial, &dcbSerialParams)) {
            std::wcerr.imbue(std::locale(""));
            std::wcerr << L"Lỗi khi lấy trạng thái cổng COM: " << GetLastError() << std::endl;
            CloseHandle(hSerial);
            return;
        }

        // Thiết lập Baud Rate từ tham số
        dcbSerialParams.BaudRate = baudRate_;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams)) {
            std::wcerr.imbue(std::locale(""));
            std::wcerr << L"Lỗi khi thiết lập trạng thái cổng COM: " << GetLastError() << std::endl;
            CloseHandle(hSerial);
            return;
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        if (!SetCommTimeouts(hSerial, &timeouts)) {
            std::wcerr.imbue(std::locale(""));
            std::wcerr << L"Lỗi khi thiết lập timeouts: " << GetLastError() << std::endl;
            CloseHandle(hSerial);
            return;
        }

        std::wcout.imbue(std::locale(""));
        std::wcout << L"Đã kết nối thành công với cổng COM: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(portName) << std::endl;
        connected = true;
#endif
    }

    ~SerialPortImpl() {
        if (connected) {
#ifdef _WIN32
            std::wcout.imbue(std::locale(""));
            std::wcout << L"Đóng cổng COM." << std::endl;
            CloseHandle(hSerial);
#endif
        }
    }

    bool isConnected() const {
        return connected;
    }

    std::string readLine() {
        if (!connected) {
            return "";
        }

        std::string line;
        char ch;
        bool lineComplete = false;

        while (!lineComplete) {
#ifdef _WIN32
            DWORD bytesRead = 0;
            if (ReadFile(hSerial, &ch, 1, &bytesRead, NULL)) {
                if (bytesRead == 1) {
                    if (ch == '\n') {
                        lineComplete = true;
                    }
                    else if (ch != '\r') {  // Ignore carriage returns
                        line += ch;
                    }
                }
                else {
                    // No data available
                    break;
                }
            }
            else {
                std::wcerr.imbue(std::locale(""));
                std::wcerr << L"Lỗi khi đọc dữ liệu từ cổng COM: " << GetLastError() << std::endl;
                break;
            }
#endif
        }

        return line;
    }

    bool writeLine(const std::string& data) {
        if (!connected) {
            return false;
        }

        std::string line = data + "\n";

#ifdef _WIN32
        DWORD bytesWritten = 0;
        if (!WriteFile(hSerial, line.c_str(), static_cast<DWORD>(line.size()), &bytesWritten, NULL)) {
            std::wcerr.imbue(std::locale(""));
            std::wcerr << L"Lỗi khi ghi dữ liệu vào cổng COM: " << GetLastError() << std::endl;
            return false;
        }
        return bytesWritten == line.size();
#endif
    }

private:
    bool connected;
    int baudRate_;

#ifdef _WIN32
    HANDLE hSerial;
#endif
};

SerialPort::SerialPort(const std::string& portName) : impl(new SerialPortImpl(portName, 9600)) {}

SerialPort::SerialPort(const std::string& portName, int baudRate) : impl(new SerialPortImpl(portName, baudRate)) {}

SerialPort::~SerialPort() = default;

bool SerialPort::isConnected() const {
    return impl->isConnected();
}

std::string SerialPort::readLine() {
    return impl->readLine();
}

bool SerialPort::writeLine(const std::string& data) {
    return impl->writeLine(data);
}
