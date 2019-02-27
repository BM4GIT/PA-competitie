#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stepper.h"

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
    void on_Startprogram_clicked();
    void on_Stop_clicked();
    void on_aantal_cm_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    bool m_stop;
    Stepper m_stepper;
    int m_keerDraaien;
};

#endif // MAINWINDOW_H
