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
    void on_startProgramma_clicked();
    void on_stopProgramma_clicked();
    void on_kooiLengte_sliderMoved(int position);
    void timeProgram();
    void dateProgram();

    void on_butVooruit_clicked();

    void on_butAchteruit_clicked();

    void on_butStop_clicked();

private:

    Ui::MainWindow *ui;
    bool m_stop;
    Stepper m_stepper;
    int m_keerDraaien;
};

#endif // MAINWINDOW_H
