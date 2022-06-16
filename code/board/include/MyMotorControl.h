#ifndef MY_MOTOR_CONTROL_H
#define MY_MOTOR_CONTROL_H

#include "mbed.h"

class MyMotorControl{
public:
    MyMotorControl(float, Mutex &mutex_motor, ConditionVariable &cond_motor, char &motor_state);
    ~MyMotorControl();
    void start();
    void front();
    void left();
    void right();
    void back();
    void go();
    void stop();
    void speedUp();
    void slowDown();
    char getState();
private:
    void setup();
private:
    DigitalOut* IN1 = nullptr; // ARDUINO_UNO_D4  = PA_3
    DigitalOut* IN2 = nullptr; // ARDUINO_UNO_D5  = PB_4
    DigitalOut* IN3 = nullptr; // ARDUINO_UNO_D6  = PB_1
    DigitalOut* IN4 = nullptr; // ARDUINO_UNO_D7  = PA_4
    PwmOut* ENA = nullptr; // ARDUINO_UNO_D2  = PD_14
    PwmOut* ENB = nullptr; // ARDUINO_UNO_D3  = PB_0
    float speed ;
    char *state;
    Thread _thread;
    Mutex *_mutex_motor = nullptr;
    ConditionVariable *_cond_motor = nullptr;
};

#endif