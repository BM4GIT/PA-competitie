#include <QCoreApplication>
#include <nextion.h>
#include <stepper.h>

Nextion nextion;
Stepper motor1;
Stepper motor2;

// de motor is een 28BYJ-48
// full step heeft 64 stapppen per rondje
// vertraging is 64x, dus 64x64=4096 stappen per rondje
#define STAPPEN 4096

void draaiMotor( int rondjes)
{
    if ( rondjes > 0 ) {
        // draai de pers een aantal rondjes omhoog
        motor1.setReverse();
        motor2.setReverse();
        motor1.start( Stepper::SC_TURN, 100, rondjes*STAPPEN);
        motor2.start( Stepper::SC_TURN, 100, rondjes*STAPPEN);
    } else
    if ( rondjes == -1 ) {
        // draai de pers steeds verder omhoog
        motor1.setReverse();
        motor2.setReverse();
        motor1.start( Stepper::SC_GO);
        motor2.start( Stepper::SC_GO);
    } else
    if ( rondjes == -2 ) {
        // draai de pers steeds verder omlaag
        motor1.setForward();
        motor2.setForward();
        motor1.start( Stepper::SC_GO);
        motor2.start( Stepper::SC_GO);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    nextion.connect();
    motor1.init( DT_UNI, STAPPEN);
    motor2.init( DT_UNI, STAPPEN);
    motor1.setPin( 21, 20, 16, 12);
    motor2.setPin( 26, 19, 13, 6);

    nextion.sendPage( "P_MENU");

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

        // er moet tekst in staan
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

            // 1 ml is 1 cm3
            // 1 rondje gaat 1 mm omhoog
            // dat is 0,1 cm3 per cm2
            // oppervlakte pers is 250 cm2
            // 1 rondje is dus 25 ml

            int rondjes = str.toInt() / 25;
            draaiMotor( rondjes);

            // wacht totdat de motoren klaar zijn
            while ( motor1.isOn() || motor2.isOn() ) {
                if ( !nextion.receive().isNull() ) {
                    motor1.setOff();
                    motor2.setOff();
                    break;
                }
            }

            // laat de volgende pagina van de nextion zien
            nextion.sendPage( page );
        } else {
            // de nextion stuurt eerst de volgende pagina op het scherm
            // en daarna een aantal ml
            // onthoud dus de volgende pagina
            page = str;
        }
    }
    return a.exec();
}
