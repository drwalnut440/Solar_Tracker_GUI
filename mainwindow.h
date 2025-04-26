#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readSerialData();
    void handleSerialError(QSerialPort::SerialPortError error);

private:
    QSerialPort *serial;
    QLabel *statusLabel;
    QLabel *tempLabel;
    QLabel *horiLabel;
    QLabel *vertLabel;
};

#endif // MAINWINDOW_H
