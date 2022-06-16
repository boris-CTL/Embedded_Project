#include "MyGattClient.h"
#include <cstdio>

MyGattClient::MyGattClient(BLE &ble, events::EventQueue &event_queue, Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer, bool &changed_ble, char* buffer_ble, bool &error_state, PinName led, char &mode): 
    _ble(&ble), _event_queue(&event_queue), ble_process(event_queue, ble), _print_sem(&sem), _mutex(&mutex), _cond(&cond), _buffer(buffer), _changed_ble(&changed_ble), _buffer_ble(buffer_ble), _error_state(&error_state), led(led), _mode(&mode){
}

MyGattClient::~MyGattClient(){
    ble_process.stop();
    led = 0;
}

void MyGattClient::start(){
    _thread.start(callback(this, &MyGattClient::setup));
    _thread_led.start(callback(this, &MyGattClient::setup_led));
    // _thread_modify.start(callback(this, &MyGattClient::setup_modify));
}

void MyGattClient::modify_BLE_device(char* device){
    _ble->gap().disconnect(ble_module.get_connection_handle_t(), ble::local_disconnection_reason_t::USER_TERMINATION);
    ble_module.stop();
    
    isConnected = false;
    ble_process.modify_peer_device_name(device);
    // ble_module.stop();

    _print_sem->acquire();
    printf("\n----------------------------------");
    printf("Change Device: ");
    printf("%s", device);
    printf("----------------------------------\n");
    _print_sem->release();
}

void MyGattClient::setup(){
    ble_module.setup(*_print_sem, *_mutex, *_cond, _buffer, isConnected, *_mode);
    ble_process.on_init(mbed::callback(&ble_module, &GattClientModule::start));
    ble_process.on_connect(mbed::callback(&ble_module, &GattClientModule::start_discovery));
    ble_process.start();
}

void MyGattClient::setup_led(){
    while (true) {
        if(!isConnected){
            led = !led;
        }
        else {
            led = 1;
        }
        ThisThread::sleep_for(500ms);
    }
}

// void MyGattClient::setup_modify(){
//     // _mutex_ble->lock();
//     while (true) {
//         // _cond_ble->wait();
//         if (*_changed_ble){
//             printf("BLE_Buffer: %s\n", _buffer_ble);
//             modify_BLE_device();
//             *_changed_ble = false;
//         }
//         ThisThread::sleep_for(500ms);
//         // _event_queue->call(callback(this, &MyGattClient::modify_BLE_device));
//     }
//     // _mutex_ble->unlock();
// }
