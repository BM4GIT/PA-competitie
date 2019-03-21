#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include "stepper.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_stepper.init();
    m_stepper.setPin( 15, 18, 14);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startProgramma_clicked()
{

    m_stop = false;
    // kies tussen test (timeProgram) of echt (dateProgram)
    if ( ui->herhaalDagen->value() ) {
        // seldt bevat de datum waarop er wordt gestart
        // curdt bevat de huidige datum
        QDateTime seldt = QDateTime( ui->startDatum->selectedDate());
        QDateTime curdt = QDateTime( QDate::currentDate());
        qint64 tm = curdt.msecsTo( seldt);
        QTimer::singleShot( tm, this, SLOT(dateProgram()));
    }
    else {
        // seldt bevat de datum waarop er wordt gestart
        // curdt bevat de huidige datum
        QDateTime seltm = QDateTime( QDate::currentDate(), ui->startTijd->time());
        QDateTime curtm = QDateTime( QDate::currentDate(), QTime::currentTime());
        qint64 tm = curtm.msecsTo( seltm);
        QTimer::singleShot( tm, this, SLOT(timeProgram()));
    }

}

void MainWindow::timeProgram()
{
    // start nu de programmering van de kooi
    if ( m_stop) return;
    // verschoon de bak
    m_stepper.turn( m_keerDraaien, 20);
    // stel de nieuwe startdatum in
    QTimer::singleShot( ui->wachtMinuten->value() * 3600000, this, SLOT(timeProgram()));
}

void MainWindow::dateProgram()
{
    // start nu de programmering van de kooi
    if ( m_stop ) return;
    // verschoon de bak
    m_stepper.turn( m_keerDraaien, 20);
    // stel de nieuwe startdatum in
    QTimer::singleShot( ui->herhaalDagen->value() * 86400000, this, SLOT(timeProgram()));
}

void MainWindow::on_stopProgramma_clicked()
{
    // m_stop wordt gebruikt in de on_Startprogram_clicked routine
    m_stop = true;
}

void MainWindow::on_kooiLengte_sliderMoved(int position)
{
    // m_keerDraaien wordt gebruikt in de on_Startprogram_clicked routine
    m_keerDraaien = position/16*200;
    ui->ingesteldeLengte->setText( QString::number( ui->kooiLengte->value()) + " cm");
}

void MainWindow::on_butVooruit_clicked()
{
    m_stepper.setForward();
    m_stepper.move( 2000);
}

void MainWindow::on_butAchteruit_clicked()
{
    m_stepper.setReverse();
    m_stepper.move( 2000);
}

void MainWindow::on_butStop_clicked()
{
    m_stepper.setOff();
}
