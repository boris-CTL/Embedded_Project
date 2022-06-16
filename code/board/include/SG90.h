#ifndef SG90_H
#define SG90_H
#include "mbed.h"
 
class SG90{
public:
    SG90(PinName pwnPin);
    void SetAngle(float fAngle);
private:
    PwmOut* Pin = nullptr; // ARDUINO_UNO_D2  = PD_14
    int iPreRotate;
};
#endif