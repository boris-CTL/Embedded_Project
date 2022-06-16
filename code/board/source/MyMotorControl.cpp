#include "MyMotorControl.h"
#include <cstdio>

MyMotorControl::MyMotorControl(float speed, Mutex &mutex_motor, ConditionVariable &cond_motor, char &motor_state): 
    speed(speed), _mutex_motor(&mutex_motor), _cond_motor(&cond_motor), state(&motor_state){
    IN1 = new DigitalOut(ARDUINO_UNO_D4);
    IN2 = new DigitalOut(ARDUINO_UNO_D5);
    IN3 = new DigitalOut(ARDUINO_UNO_D6);
    IN4 = new DigitalOut(ARDUINO_UNO_D7);
    ENA = new PwmOut(ARDUINO_UNO_D2);
    ENB = new PwmOut(ARDUINO_UNO_D3);
}

MyMotorControl::~MyMotorControl(){
    delete [] IN1;
    delete [] IN2;
    delete [] IN3;
    delete [] IN4;
    delete [] ENA;
    delete [] ENB;
}

void MyMotorControl::start(){
    _thread.start(callback(this, &MyMotorControl::setup));
}

void MyMotorControl::front(){
    *IN1 = 1;
    *IN2 = 0;
    *IN3 = 0;
    *IN4 = 1;
}

void MyMotorControl::left(){
    *IN1 = 0;
    *IN2 = 1;
    *IN3 = 0;
    *IN4 = 1;
}

void MyMotorControl::right(){
    *IN1 = 1;
    *IN2 = 0;
    *IN3 = 1;
    *IN4 = 0;
}

void MyMotorControl::back(){
    *IN1 = 0;
    *IN2 = 1;
    *IN3 = 1;
    *IN4 = 0;
}

void MyMotorControl::go(){
    ENA->write(speed);
    ENB->write(speed);
}

void MyMotorControl::stop(){
    ENA->write(0.0f);
    ENB->write(0.0f);
}

void MyMotorControl::speedUp(){
    speed += 0.05;
    if(speed>1){
        speed = 1;
    }
    ENA->write(speed);
    ENB->write(speed);
}

void MyMotorControl::slowDown(){
    speed -= 0.05;
    if(speed<0){
        speed = 0;
    }
    ENA->write(speed);
    ENB->write(speed);
}

char MyMotorControl::getState(){
    return *state;
}

void MyMotorControl::setup(){
    *IN1 = 0;
    *IN2 = 0;
    *IN3 = 0;
    *IN4 = 0;
    ENA->period(0.05f);
    ENB->period(0.05f);
    ENA->write(0.0f);
    ENB->write(0.0f);
    _mutex_motor->lock();
    while (true) {
        _cond_motor->wait();
        switch (*state) {
        case 'G':
            this->go();
            printf("Go Executed!\n");
            break;
        case 'S':
            this->stop();
            printf("Stop Executed!\n");
            break;
        case 'L':
            this->left();
            printf("Left Executed!\n");
            break;
        case 'R':
            this->right();
            printf("Right Executed!\n");
            break;
        case 'F':
            this->front();
            printf("Front Executed!\n");
            break;
        case 'B':
            this->back();
            printf("Back Executed!\n");
            break;
        case 'U':
            this->speedUp();
            printf("Speedup Executed!\n");
            break;
        case 'D':
            this->slowDown();
            printf("Slowdown Executed!\n");
            break;
        }
    }
    _mutex_motor->unlock();
}