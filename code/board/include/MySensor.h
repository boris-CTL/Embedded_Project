#ifndef MY_SENSOR_H
#define MY_SENSOR_H

#include "mbed.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

class MySensor{
public:
    MySensor(Mutex &mutex, ConditionVariable &cond, char* buffer);
    ~MySensor();
    void start();
private:
    void setup();
private:
    float sensor_value = 0;
    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};

    Thread _thread;
    ConditionVariable* _cond = nullptr;
    Mutex* _mutex = nullptr;
    char* _buffer;
};

#endif