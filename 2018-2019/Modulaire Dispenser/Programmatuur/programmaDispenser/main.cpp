#include <QCoreApplication>
#include <nextion.h>
#include <stepper.h>

Nextion nextion;
Stepper motor1;
Stepper motor2;

void draaiMotor( int rondjes)
{
    if ( rondjes > 0 ) {
        motor1.start( Stepper::SC_TURN, 100, rondjes*512);
        motor2.start( Stepper::SC_TURN, 100, rondjes*512);
    } else
    if ( rondjes == -1 ) {
        motor1.setForward();
        motor2.setForward();
        motor1.start( Stepper::SC_GO);
        motor2.start( Stepper::SC_GO);
    } else
    if ( rondjes == -2 ) {
        motor1.setReverse();
        motor2.setReverse();
        motor1.start( Stepper::SC_GO);
        motor2.start( Stepper::SC_GO);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    nextion.connect();
    motor1.init( DT_ULN2003, 512);
    motor2.init( DT_ULN2003, 512);
    motor1.setPin( 21, 20, 16, 12);
    motor2.setPin( 26, 19, 13, 6);

    QVariant var;
    QString str;
    QString page;

    forever {
        // haal informatie van de nextion
        var = nextion.receive();
        // het moet goede informatie zijn
        if ( var.isNull() )
            continue;
        // maak er text van
        str = var.toString();
        // het mag geen lege tekst zijn
        if ( str.isEmpty() )
            continue;

        // check wat de nextion heeft verzonden
        if ( str == "NEER" ) {
            draaiMotor( -2);
        } else
        if ( str == "OP" ) {
            draaiMotor( -1);
        } else
        if ( str == "STOP" ) {
            motor1.setOff();
            motor2.setOff();
        } else
        if ( str.toInt() > 0 ) {
            // dit is het aantal ml
            // 1 rondje van de motor is ongeveer 10 ml
            int rondjes = str.toInt() / 10;
            draaiMotor( rondjes);
            // wacht totdat de motoren klaar zijn
            while ( motor1.isOn() || motor2.isOn() );
            // laat de volgende pagina van de nextion zien
            nextion.sendPage( page );
        } else {
            // onthoud de volgende pagina voor de nextion
            page = str;
        }
    }
    return a.exec();
}
