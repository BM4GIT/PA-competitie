#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include "stepper.h"
#include <QTimer>

// omrekenen lengte kooi (cm) naar aantal rotaties
// m_keerDraaien = lengte * FACTOR
#define FACTOR 20

Stepper stepper1;
Stepper stepper2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle( "Clean Cage");

    stepper1.init();
    stepper1.setPin( 27, 17, 22);
    stepper2.init();
    stepper2.setPin( 26, 16, 21);

    // instellen van de isr en timer events
    wiringPiISR( 5, INT_EDGE_RISING, &on_kooiknop);
    connect( &m_timerTm, SIGNAL(timeout()), this, SLOT(timeProgram()));
    connect( &m_timerDt, SIGNAL(timeout()), this, SLOT(dateProgram()));

    stepper1.setHold( false);
    stepper2.setHold( false);

    m_keerDraaien=50*FACTOR;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startProgramma_clicked()
{
    // kies tussen test (timeProgram) of echt (dateProgram)
    if ( ui->herhaalDagen->value() ) {
        QDateTime seldt = QDateTime( ui->startDatum->selectedDate());
        QDateTime curdt = QDateTime( QDate::currentDate());
        qint64 tm = curdt.msecsTo( seldt);
        m_timerDt.setSingleShot( true);
        m_timerDt.start( tm); // start dateProgram
    }
    else {
        QDateTime seltm = QDateTime( QDate::currentDate(), ui->startTijd->time());
        QDateTime curtm = QDateTime( QDate::currentDate(), QTime::currentTime());
        qint64 tm = curtm.msecsTo( seltm);
        m_timerTm.setSingleShot( true);
        m_timerTm.start( tm); // start timeProgram
    }
}

void MainWindow::timeProgram()
{
    // verschoon de bak
    stepper1.setForward();
    stepper2.setReverse();
    stepper1.start( Stepper::SC_TURN, 1, m_keerDraaien);
    stepper2.start( Stepper::SC_TURN, 1, m_keerDraaien);

    // stel de nieuwe starttijd in
    int n = ui->wachtSeconden->value() * 1000;
    m_timerTm.start( n);
}

void MainWindow::dateProgram()
{
    // verschoon de bak
    stepper1.setForward();
    stepper2.setReverse();
    stepper1.start( Stepper::SC_TURN, 1, m_keerDraaien);
    stepper2.start( Stepper::SC_TURN, 1, m_keerDraaien);

    // stel de nieuwe startdatum in
    QDateTime seldt = QDateTime( ui->startDatum->selectedDate());
    QDateTime curdt = QDateTime( QDate::currentDate());
    qint64 tm = curdt.msecsTo( seldt);
    m_timerDt.start( tm);
}

void MainWindow::on_kooiLengte_sliderMoved(int position)
{
    // m_keerDraaien wordt gebruikt in de timeProgram en dateProgram routines
    m_keerDraaien = position*FACTOR;
    ui->ingesteldeLengte->setText( QString::number( ui->kooiLengte->value()) + " cm");
}

void MainWindow::on_butVooruit_clicked()
{
    stepper1.setReverse();
    stepper2.setForward();
    stepper1.start( Stepper::SC_GO, 1);
    stepper2.start( Stepper::SC_GO, 1);
}

void MainWindow::on_butAchteruit_clicked()
{
    stepper1.setForward();
    stepper2.setReverse();
    stepper1.start( Stepper::SC_GO, 1);
    stepper2.start( Stepper::SC_GO, 1);
}

void MainWindow::on_butStop_clicked()
{
    stepper1.setOff();
    stepper2.setOff();
    m_timerTm.stop();
    m_timerDt.stop();
}

void MainWindow::on_kooiknop()
{
    // handbediening
    if ( stepper1.isOn() ) {
        stepper1.setOff();
        stepper2.setOff();
    }
    else {
        stepper1.setForward();
        stepper2.setReverse();
        stepper1.start( Stepper::SC_GO, 1);
        stepper2.start( Stepper::SC_GO, 1);
    }
}
