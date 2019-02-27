#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include "stepper.cpp"
#include "stepper.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Startprogram_clicked()
{
    m_stop = false;
    // start nu de programmering van de kooi
    while ( true ) {
        // seldt bevat de datum waarop er wordt gestart
        // curdt bevat de huidige datum
        QDate seldt = ui->calendarWidget->selectedDate();
        QDate curdt = QDate::currentDate();
        while ( curdt < seldt ) {
            // wacht totdat de startdatum is bereikt
            // stop als op de stopknop wordt gedrukt
            if ( m_stop ) break;
        }
        if ( m_stop ) break;
        // verschoon de bak
        m_stepper.turn( m_keerDraaien);
        // stel de nieuwe startdatum in
        seldt.addDays( ui->herhaaldagen->value());
    }
}

void MainWindow::on_Stop_clicked()
{
    // m_stop wordt gebruikt in de on_Startprogram_clicked routine
    m_stop = true;
}

void MainWindow::on_aantal_cm_sliderMoved(int position)
{
    // m_keerDraaien wordt gebruikt in de on_Startprogram_clicked routine
    m_keerDraaien = position/16;
    ui->lblCm->setText( QString::number( m_keerDraaien) + " cm");
}
