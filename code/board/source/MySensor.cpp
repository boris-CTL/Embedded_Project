#include "MySensor.h"
#include "dest.h"

MySensor::MySensor(Mutex &mutex, ConditionVariable &cond, char* buffer):
    _cond(&cond), _mutex(&mutex), _buffer(buffer){
}

MySensor::~MySensor(){
}

void MySensor::start(){
    _thread.start(callback(this, &MySensor::setup));
}

void MySensor::setup(){
    printf("Start sensor init\n");

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();

    BSP_MAGNETO_Init();
    BSP_GYRO_Init();
    BSP_ACCELERO_Init();

    while(1) {
        _mutex->lock();
        int len = sprintf(_buffer, "{\"type\":\"STM_SENSOR\", ");
        sensor_value = BSP_TSENSOR_ReadTemp();
        len += sprintf(_buffer+len, "\"T\":%.2f, ", sensor_value);
        // printf("\nTEMPERATURE = %.2f degC\n", sensor_value);

        sensor_value = BSP_HSENSOR_ReadHumidity();
        len += sprintf(_buffer+len, "\"H\":%.2f, ", sensor_value);
        // printf("HUMIDITY    = %.2f %%\n", sensor_value);

        sensor_value = BSP_PSENSOR_ReadPressure();
        len += sprintf(_buffer+len, "\"P\":%.2f, ", sensor_value);
        // printf("PRESSURE is = %.2f mBar\n", sensor_value);

        BSP_MAGNETO_GetXYZ(pDataXYZ);
        len += sprintf(_buffer+len, "\"Mag\":{\"X\":%d, \"Y\":%d, \"Z\":%d}, ", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2]);
        // printf("\nMAGNETO_X = %d\n", pDataXYZ[0]);
        // printf("MAGNETO_Y = %d\n", pDataXYZ[1]);
        // printf("MAGNETO_Z = %d\n", pDataXYZ[2]);

        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        len += sprintf(_buffer+len, "\"Gyr\":{\"X\":%.2f, \"Y\":%.2f, \"Z\":%.2f}, ", pGyroDataXYZ[0], pGyroDataXYZ[1], pGyroDataXYZ[2]);
        // printf("\nGYRO_X = %.2f\n", pGyroDataXYZ[0]);
        // printf("GYRO_Y = %.2f\n", pGyroDataXYZ[1]);
        // printf("GYRO_Z = %.2f\n", pGyroDataXYZ[2]);

        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        len += sprintf(_buffer+len, "\"Acc\":{\"X\":%d, \"Y\":%d, \"Z\":%d}, \"dest\":\"%s\"}", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], DEST);
        // printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
        // printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
        // printf("ACCELERO_Z = %d\n", pDataXYZ[2]);

        _cond->notify_all();
        _mutex->unlock();
        ThisThread::sleep_for(10s);
    }
}