#ifndef MY_RADAR_H
#define MY_RADAR_H

#include "mbed.h"
#include "dest.h"
#include "SG90.h"
#include "HC_SR04.h"

class MyRadar{
public:
    MyRadar(Mutex &mutex, ConditionVariable &cond, char* buffer, PinName pinServo, PinName pinTrig, PinName pinEcho, char &mode);
    ~MyRadar();
    void start();
private:
    void setupServo();
    void setupRadar();
private:
    SG90 servo;
    float angle = 0;
    float deltaAngle = 1;
    bool isReverse = false;

    HC_SR04 radar;
    int dist = 0;

    Thread _threadServo;
    Thread _threadRadar;

    ConditionVariable* _cond = nullptr;
    Mutex* _mutex = nullptr;

    char* _buffer;
    // bool* _startRadar;
    char* _mode;
};

#endif