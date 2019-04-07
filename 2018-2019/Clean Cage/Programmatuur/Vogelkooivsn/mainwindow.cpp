#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDate>
#include "stepper.h"
#include <QTimer>

// omrekenen lengte kooi (cm) naar aantal rotaties
// m_keerDraaien = lengte * FACTOR
#define FACTOR 5000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle( "Clean Cage");

    pinMode( 5, INPUT);

    m_stepper1.init();
    m_stepper1.setPin( 27, 17, 22);
    m_stepper2.init();
    m_stepper2.setPin( 26, 16, 21);

    connect( &m_timerTm, SIGNAL(timeout()), this, SLOT(timeProgram()));
    connect( &m_timerDt, SIGNAL(timeout()), this, SLOT(dateProgram()));
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

	// instellen op handbediening
    while ( true ) {
        if ( digitalRead( 5) ) {
            m_stepper1.setForward();
            m_stepper2.setReverse();
            m_stepper1.start( Stepper::SC_GO, 1);
            m_stepper2.start( Stepper::SC_GO, 1);
        }
        else {
            m_stepper1.setOff();
            m_stepper2.setOff();
        }
    }
}

void MainWindow::timeProgram()
{
    // verschoon de bak
    m_stepper1.setForward();
    m_stepper2.setReverse();
    m_stepper1.start( Stepper::SC_TURN, 1, m_keerDraaien);
    m_stepper2.start( Stepper::SC_TURN, 1, m_keerDraaien);

    // stel de nieuwe starttijd in
    int n = ui->wachtMinuten->value() * 60 * 1000;
    m_timerTm.start( n);
}

void MainWindow::dateProgram()
{
    // verschoon de bak
    m_stepper1.setForward();
    m_stepper2.setReverse();
    m_stepper1.start( Stepper::SC_TURN, 1, m_keerDraaien);
    m_stepper2.start( Stepper::SC_TURN, 1, m_keerDraaien);

    // stel de nieuwe startdatum in
    QDateTime seldt = QDateTime( ui->startDatum->selectedDate());
    QDateTime curdt = QDateTime( QDate::currentDate());
    qint64 tm = curdt.msecsTo( seldt);
    m_timerDt.start( tm);
}

void MainWindow::on_kooiLengte_sliderMoved(int position)
{
    // m_keerDraaien wordt gebruikt in de on_Startprogram_clicked routine
    m_keerDraaien = position*FACTOR;
    ui->ingesteldeLengte->setText( QString::number( ui->kooiLengte->value()) + " cm");
}

void MainWindow::on_butVooruit_clicked()
{
    m_stepper1.setReverse();
    m_stepper2.setForward();
    m_stepper1.start( Stepper::SC_GO, 1);
    m_stepper2.start( Stepper::SC_GO, 1);
}

void MainWindow::on_butAchteruit_clicked()
{
    m_stepper1.setForward();
    m_stepper2.setReverse();
    m_stepper1.start( Stepper::SC_GO, 1);
    m_stepper2.start( Stepper::SC_GO, 1);
}

void MainWindow::on_butStop_clicked()
{
    m_stepper1.setOff();
    m_stepper2.setOff();
}
