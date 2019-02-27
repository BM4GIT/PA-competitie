// file:   stepper.h
// author: D.E.Veloper
// date:   2020-01-01

#ifndef STEPPER_H
#define STEPPER_H

#include "actuator.h"

#define DT_UNIPOLAR	   0
#define DT_A4988   	   1

class Stepper : public Actuator
{
public:
    Stepper();
    ~Stepper();

    void init( int dt = DT_A4988, unsigned int stepsPerRotation = 200);

    void setPin( unsigned int pin1,
                 unsigned int pin2,
                 unsigned int pin3,
                 unsigned int pin4);
    void setPin( unsigned int pinStep,
                 unsigned int pinDir,
                 unsigned int pinEnab);

    void setForward( bool forward = true);
    void setReverse();
    void setHold( bool hold = true);
    void setRelease();

    void turn( unsigned int steps, unsigned int speed = 100);
    void rotate( unsigned int angle, unsigned int speed = 100); // angle in degr
    void move( unsigned int time, unsigned int speed = 100);    // time in msec
    void setOff();

private:
    void stepUniPolar();
    void stepBiPolar();

    int  m_dt;
    int  m_pin1;
    int  m_pin2;
    int  m_pin3;
    int  m_pin4;
    int  m_stepspr; // steps per rotation
    bool m_forward; // true = forward, false = reverse
    bool m_hold;    // true = hold torque, false = release torque
    int  m_step;
};

#endif // STEPPER_H