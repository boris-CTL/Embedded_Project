#ifndef MY_GATT_CLIENT_H
#define MY_GATT_CLIENT_H

#include "events/EventQueue.h"
#include "ble/GattClient.h"
#include "GattClientModule.h"

#include "gatt_client_process.h"
#include "mbed-trace/mbed_trace.h"
#include <cstddef>
#include <string>

class MyGattClient{
public:
    MyGattClient(BLE &ble, events::EventQueue &event_queue, Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer, bool &changed_ble, char* buffer_ble, bool &error_state, PinName led, char &mode);
    ~MyGattClient();
    void start();
    void modify_BLE_device(char* device);
private:
    void setup();
    void setup_led();
    void setup_modify();
private:
    DigitalOut led;

    BLE *_ble = nullptr;
    events::EventQueue *_event_queue = nullptr;
    GattClientModule ble_module;
    GattClientProcess ble_process;
    Thread _thread;
    Thread _thread_led;
    Thread _thread_modify;
    Semaphore *_print_sem = nullptr;

    Mutex *_mutex = nullptr;
    ConditionVariable *_cond = nullptr;
    char* _buffer;

    // Mutex *_mutex_ble = nullptr;
    // ConditionVariable *_cond_ble = nullptr;
    char* _buffer_ble;
    bool* _changed_ble;

    bool* _error_state;
    bool isConnected = false;
    char* _mode;
};

#endif