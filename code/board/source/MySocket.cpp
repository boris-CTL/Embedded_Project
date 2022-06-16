#include "MySocket.h"
#include <cstdio>
#include "dest.h"

MySocket::MySocket(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer, Mutex &mutex_motor, ConditionVariable &cond_motor, char &motor_state, bool &changed_ble, char* buffer_ble, PinName led, bool &error_state, MyGattClient &myGattClient, char &mode, MyAudio &myAudio):
    _sem(&sem), _mutex(&mutex), _cond(&cond), _buffer(buffer), _mutex_motor(&mutex_motor), _cond_motor(&cond_motor), _motor_state(&motor_state), _changed_ble(&changed_ble), _buffer_ble(buffer_ble), led(led), _error_state(&error_state), _myGattClient(&myGattClient), _mode(&mode), _myAudio(&myAudio){
    // this->led = 1;
    _socket = new SocketModule(*_sem, *_mutex, *_cond, *_mutex_motor, *_cond_motor, *_motor_state, *_changed_ble, _buffer_ble, *_myGattClient, *_mode, *_myAudio);
    // _socket_state = _socket->run();
    // printf("Socket State: %d\n", _socket_state);
    // this->led = 0;
}

MySocket::~MySocket(){
    delete _socket;
}

void MySocket::start(){
    _socket_state = _socket->run();
    printf("Socket State: %d\n", _socket_state);

    _threadSend = new Thread();
    _threadSend->start(callback(this, &MySocket::setupSend));
    _threadRecv = new Thread();
    _threadRecv->start(callback(this, &MySocket::setupRecv));

    // _socket_state = _socket->run();
    // printf("Socket State: %d\n", _socket_state);
}

void MySocket::restart(){
    printf("Socket Thread restart!\n");
    delete _threadSend;
    delete _threadRecv;
    // delete _socket;
    // _socket = nullptr;
    _threadSend = new Thread();
    _threadSend->start(callback(this, &MySocket::setupSend));
    _threadRecv = new Thread();
    _threadRecv->start(callback(this, &MySocket::setupRecv));
}

void MySocket::setupSend(){
    // led = 1;
    // _socket = new SocketModule(*_sem, *_mutex, *_cond, *_mutex_motor, *_cond_motor, *_motor_state, *_changed_ble, _buffer_ble, *_myGattClient, *_mode, *_myAudio);
    printf("\r\nStarting socket thread\r\n\r\n");
    // while (_socket==nullptr) {
    //     // printf("Looping\r\n");
    //     // led = !led;
    //     ThisThread::sleep_for(500ms);
    //     _socket = new SocketModule(*_sem, *_mutex, *_cond, *_mutex_motor, *_cond_motor, *_motor_state, *_changed_ble, _buffer_ble, *_myGattClient, *_mode, *_myAudio);
    // }
    // bool _socket_state = _socket->run();
    // printf("Socket State: %d\n", _socket_state);
    // led = 0;
    if (!_socket_state){
        *_error_state = true;
        while (true) {
            // led = !led;
            ThisThread::sleep_for(500ms);
        }
    }
    socketInit = true;

    int len = sprintf(_buffer, "{\"type\":\"STM_SETUP\", \"ID\":\"%s\"}", STM_ID);
    _socket->send_http_request(_buffer);

    _mutex->lock();
    while (true) {
        _cond->wait();
        bool response = _socket->send_http_request(_buffer);
        if (!response){
            printf("Error sending: %d\n", response);
            socketInit = false;
            *_error_state = true;
            delete _socket;
            break;
        }
        ThisThread::sleep_for(5ms);
    }
    _mutex->unlock();
}

void MySocket::setupRecv(){
    while (true) {
        if(socketInit){
            break;
        }
        else {
            led = !led;
            ThisThread::sleep_for(500ms);
        }
    }
    led = 0;
    printf("Start Receive HTTP Response!\n");
    while (true) {
        bool response = _socket->receive_http_response();
        if(!socketInit||!response){
            break;
        }
    }
}