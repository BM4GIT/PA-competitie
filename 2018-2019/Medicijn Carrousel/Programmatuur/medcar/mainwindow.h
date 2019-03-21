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

private slots:
    void on_pbStart_clicked();

private:
    Ui::MainWindow *ui;
    void waitTime( QTime time);
    void buzz();
    void on_Ochtend();
    void on_Middag();
    void on_Avond();

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
