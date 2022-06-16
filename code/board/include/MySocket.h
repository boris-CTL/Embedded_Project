#ifndef MY_SOCKET_H
#define MY_SOCKET_H

#include "mbed.h"
#include "SocketModule.h"
#include "MyGattClient.h"
#include "MyAudio.h"

class MySocket{
public:
    MySocket(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer, Mutex &mutex_motor, ConditionVariable &cond_motor, char &motor_state, bool &changed_ble, char* buffer_ble, PinName led, bool &error_state, MyGattClient &myGattClient, char &mode, MyAudio &myAudio);
    ~MySocket();
    void start();
    void restart();
private:
    void setupSend();
    void setupRecv();
private:
    DigitalOut led;

    SocketModule *_socket = nullptr;
    Thread* _threadSend;
    Thread* _threadRecv;
    Semaphore *_sem = nullptr;

    Mutex *_mutex = nullptr;
    ConditionVariable *_cond = nullptr;

    Mutex *_mutex_motor = nullptr;
    ConditionVariable *_cond_motor = nullptr;

    // Mutex *_mutex_ble = nullptr;
    // ConditionVariable *_cond_ble = nullptr;
    char* _buffer_ble;
    bool* _changed_ble;
    MyGattClient *_myGattClient;

    char* _buffer;
    volatile bool socketInit = false;
    char* _motor_state;
    bool* _error_state;
    // bool* _startRadar;
    char* _mode;
    MyAudio *_myAudio;
    bool _socket_state = false;
};

#endif