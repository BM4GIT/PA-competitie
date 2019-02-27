#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "buzzer.h"
#include "led.h"
#include "stepper.h"
#include "sensor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Buzzer  m_buzzer;
    Led     m_ledO;
    Led     m_ledM;
    Led     m_ledA;
    Sensor  m_sensorO;
    Sensor  m_sensorM;
    Sensor  m_sensorA;
    Sensor  m_button;
    Stepper m_stepper;
};

#endif // MAINWINDOW_H
