#include <QCoreApplication>
#include <wiringPi.h>

void zoemer( int tijd)
{
        digitalWrite(20,HIGH);
        delay(tijd);
        digitalWrite(20,LOW);
        delay(tijd);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //////////////////////////////////////////////

    wiringPiSetupGpio();

    pinMode(20,OUTPUT); // voor zoemer
    pinMode(10,INPUT);  // de sensor

    forever {
        if ( !digitalRead(10) ) {
            zoemer(1000);
        }
    }

    //////////////////////////////////////////////

    return a.exec();
}
