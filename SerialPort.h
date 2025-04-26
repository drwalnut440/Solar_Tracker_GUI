#ifndef SOLAR_TRACKER_SERIALPORT_H
#define SOLAR_TRACKER_SERIALPORT_H

#include <string>
#include <memory>

class SerialPortImpl;

class SerialPort {
public:
    explicit SerialPort(const std::string& portName);
    explicit SerialPort(const std::string& portName, int baudRate); // Đã thêm constructor này
    ~SerialPort();

    bool isConnected() const;
    std::string readLine();
    bool writeLine(const std::string& data);

private:
    std::unique_ptr<SerialPortImpl> impl;
};

#endif // SOLAR_TRACKER_SERIALPORT_H