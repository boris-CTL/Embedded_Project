#include "MyRadar.h"
#include <cstdio>

MyRadar::MyRadar(Mutex &mutex, ConditionVariable &cond, char* buffer, PinName pinServo, PinName pinTrig, PinName pinEcho, char &mode):
    _mutex(&mutex), _cond(&cond), _buffer(buffer), servo(pinServo), radar(pinTrig, pinEcho, 0.1, 1, dist), _mode(&mode){
    // radar(pinTrig, pinEcho, 0.1, 1, &MyRadar::onRadarUpdate);
    // radar(pinTrig, pinEcho, 0.1, 1, dist);
}

MyRadar::~MyRadar(){

}

void MyRadar::start(){
    _threadServo.start(callback(this, &MyRadar::setupServo));
    _threadRadar.start(callback(this, &MyRadar::setupRadar));
}

void MyRadar::setupServo(){
    servo.SetAngle(angle);
    while (true) {
        if(*_mode=='R'){
            if(isReverse){
                angle -= deltaAngle;
            }
            else{
                angle += deltaAngle;
            }
            if(angle>=90){
                angle = 90;
                isReverse = true;
            }
            else if(angle<=-90){
                angle = -90;
                isReverse = false;
            }
            // printf("Angle: %f\n", angle);
            servo.SetAngle(angle);
        }
        ThisThread::sleep_for(60ms);
    }
}

void MyRadar::setupRadar(){
    radar.startUpdates();
    while(true)
    {
        //Do something else here
        if(*_mode=='R'){
            if(radar.checkDistance()){
                _mutex->lock();
                // printf("Angle: %f, Dist: %d\n", angle+90, dist);
                int len = sprintf(_buffer, "{\"type\":\"STM_RADAR\", \"Angle\":%f, \"Dist\":%d, \"dest\":\"%s\"}", angle+90, dist, DEST);
                _cond->notify_all();
                _mutex->unlock();
            }
        }
        ThisThread::sleep_for(100ms);
    }
}