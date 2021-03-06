#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
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

    static void on_kooiknop();

private slots:
    void on_startProgramma_clicked();
    void on_kooiLengte_sliderMoved(int position);
    void timeProgram();
    void dateProgram();
    void on_butVooruit_clicked();
    void on_butAchteruit_clicked();
    void on_butStop_clicked();

private:

    Ui::MainWindow *ui;
    bool m_stop;
    int m_keerDraaien;
    QTimer  m_timerTm;
    QTimer  m_timerDt;
    bool    m_timerCheck;
};

#endif // MAINWINDOW_H
