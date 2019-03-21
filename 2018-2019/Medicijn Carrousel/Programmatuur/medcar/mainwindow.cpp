#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QDate>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sensorA.setPin( 11);
    m_ledA.setPin( 21);
    m_sensorM.setPin( 9);
    m_ledM.setPin( 20);
    m_sensorO.setPin( 10);
    m_ledO.setPin( 16);
    m_stepper.setPin( 6, 13, 19, 26);
    m_stepper.init( 515);
    m_buzzer.setPin( 12);
    m_button.setPin( 23);

    m_buzzer.setOff();
    m_ledA.setOff();
    m_ledM.setOff();
    m_ledO.setOff();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbStart_clicked()
{
    if ( ui->cbTest->isChecked() ) {
        delay( 1000);
        setWindowTitle( "Vullen");
        m_stepper.rotate(45);

        if ( ui->cbOchtend->isChecked() ) {
            delay( 3000);
            m_ledO.setOn();
            setWindowTitle( "Neem");
            m_buzzer.setOn();
            delay( 200);
            m_buzzer.setOff();
            while ( m_sensorO.readPin() );
            delay( 1000);
            setWindowTitle( "Terug");
            while ( !m_sensorO.readPin() );
            m_ledO.setOff();
        }

        if ( ui->cbMiddag->isChecked() ) {
            delay( 3000);
            m_ledM.setOn();
            setWindowTitle( "Neem");
            m_buzzer.setOn();
            delay( 200);
            m_buzzer.setOff();
            while ( m_sensorM.readPin() );
            delay( 1000);
            setWindowTitle( "Terug");
            while ( !m_sensorM.readPin() );
            m_ledM.setOff();
        }

        if ( ui->cbAvond->isChecked() ) {
            delay( 3000);
            m_ledA.setOn();
            setWindowTitle( "Neem");
            m_buzzer.setOn();
            delay( 200);
            m_buzzer.setOff();
            while ( m_sensorA.readPin() );
            delay( 1000);
            setWindowTitle( "Terug");
            while ( !m_sensorA.readPin() );
            m_ledA.setOff();
        }

        setWindowTitle( "MedCar");
        return;
    }

    QDate today = QDate::currentDate();
    while ( true )
    {
        if ( ui->cbTest->isChecked() ) {
            delay( 1000);
            m_ledA.setOff();
            m_ledM.setOff();
            m_ledO.setOff();
            QTime tm = QTime::currentTime();
            tm = tm.addSecs( 15);
            ui->tmDraai->setTime( tm);
            ui->tmDraai->repaint();
            tm = tm.addSecs( 15);
            ui->tmOchtend->setTime( tm);
            tm = tm.addSecs( 15);
            ui->tmMiddag->setTime( tm);
            ui->tmMiddag->repaint();
            tm = tm.addSecs( 15);
            ui->tmAvond->setTime( tm);
            ui->tmAvond->repaint();
        }
        else {
            // wacht op de volgende dag
            while ( today >= QDate::currentDate() ) {
                // als tijdens het wachten op de knop wordt gedrukt
                // moet er naar de volgende stand worden gedraaid
                // (dit gebeurt bij het vullen)
                if ( m_button.readPin() )
                    m_stepper.rotate(45);
            }
        }

        // wacht op de tijd om te draaien
        waitTime( ui->tmDraai->time());
        // vul de bakjes opnieuw
        m_stepper.rotate(45);

        if ( ui->cbOchtend->isChecked() ) {
            // wacht op de ochtend tijd
            waitTime( ui->tmOchtend->time());
            // waarschuw voor het innemen
            on_Ochtend();
        }

        if ( ui->cbMiddag->isChecked() ) {
            // wacht op de middag tijd
            waitTime( ui->tmMiddag->time());
            // waarschuw voor het innemen
            on_Middag();
        }

        if ( ui->cbAvond->isChecked() ) {
            // wacht op de avond tijd
            waitTime( ui->tmAvond->time());
            // waarschuw voor het innemen
            on_Avond();
        }
    }
}

void MainWindow::waitTime( QTime time)
{
    // als tijdens het wachten op de knop wordt gedrukt
    // mowaitTimeet er naar de volgende stand worden gedraaid
    // (dit gebeurt bij het vullen)
    while ( time >= QTime::currentTime() ) {
        if ( m_button.readPin() )
            m_stepper.rotate(45);
    }
}

void MainWindow::buzz()
{
    m_buzzer.setOn();
    delay( 1000);
    m_buzzer.setOff();
    delay( 1000);
    m_buzzer.setOn();
    delay( 1000);
    m_buzzer.setOff();
}

void MainWindow::on_Ochtend()
{
    m_ledO.setOn();
    QTime tm = QTime::currentTime();
    while ( m_sensorO.readPin() == HIGH ) {
        buzz();
        tm = tm.addSecs( ui->spBuzzer->value()*60);
        waitTime( tm);
    }
    while ( m_sensorO.readPin() == LOW ) {
        buzz();
        tm = tm.addSecs( ui->spBuzzer->value()*60);
        waitTime( tm);
    }
    m_ledO.setOff();
}

void MainWindow::on_Middag()
{
    m_ledM.setOn();
    QTime tm = QTime::currentTime();
    while ( m_sensorM.readPin() == HIGH ) {
        buzz();
        tm = tm.addSecs( ui->spBuzzer->value()*60);
        waitTime( tm);
    }
    while ( m_sensorM.readPin() == LOW ) {
        buzz();
        tm = tm.addSecs( ui->spBuzzer->value()*60);
        waitTime( tm);
    }
    m_ledM.setOff();
}

void MainWindow::on_Avond()
{
    m_ledA.setOn();
    QTime tm = QTime::currentTime();
    while ( m_sensorA.readPin() == HIGH ) {
        buzz();
        tm = tm.addSecs( ui->spBuzzer->value()*60);
        waitTime( tm);
    }
    while ( m_sensorA.readPin() == LOW ) {
        buzz();
        tm = tm.addSecs( ui->spBuzzer->value()*60);
        waitTime( tm);
    }
    m_ledA.setOff();
}


