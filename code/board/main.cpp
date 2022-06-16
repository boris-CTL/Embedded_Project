#include "mbed.h"
#include "mbed-trace/mbed_trace.h"

#include "MySocket.h"
#include "MyGattClient.h"
#include "MySensor.h"
#include "MyMotorControl.h"
#include "MyRadar.h"
#include "MyAudio.h"
#include "stm32l475e_iot01_audio.h"

#include <cstdio>
#include <string>

EventQueue event_queue;

Semaphore print_sem(1);
Semaphore audio_sem(1);

Mutex mutex_socket;
ConditionVariable cond_socket(mutex_socket);
char socket_buffer[1024];
// char* socket_buffer = (char*)calloc(AUDIO_SAMPLING_FREQUENCY, sizeof(char));

Mutex mutex_motor;
ConditionVariable cond_motor(mutex_motor);

// Mutex mutex_ble;
// ConditionVariable cond_ble(mutex_ble);
char* ble_buffer;
bool ble_changed = false;

bool start_radar = false;
char mode = 'N';    // N: unknown, A: audio, R: radar

BLE &ble1 = BLE::Instance();

// G: go, S: stop, L: left, R: right, F: front, B: back, U: speedup, D: slowdown
char motor_state = 'S';

bool error_socket = false;
bool error_ble = false;

MySocket* mySocket;
MySensor* mySensor;
MyGattClient* myGattClient;
MyMotorControl* myMotorControl;
MyRadar* myRadar;
MyAudio* myAudio;

InterruptIn button(BUTTON1);

void error(){
    mySocket->restart();
    if(error_socket){
        error_socket = false;
    }
    else if(error_ble){

    }
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance){
    myAudio->BSP_AUDIO_IN_HalfTransfer_CallBack(Instance);
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance){
    myAudio->BSP_AUDIO_IN_TransferComplete_CallBack(Instance);
}

void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance){
    myAudio->BSP_AUDIO_IN_Error_CallBack(Instance);
}

int main()
{
    // #undef MBED_CONF_NSAPI_DEFAULT_WIFI_SSID
    // #define MBED_CONF_NSAPI_DEFAULT_WIFI_SSID "edimax_2.4G"

    // #undef MBED_CONF_NSAPI_DEFAULT_WIFI_PASSWORD
    // #define MBED_CONF_NSAPI_DEFAULT_WIFI_PASSWORD "0925385005"

    // #undef MBED_CONF_APP_HOSTNAME
    // #define MBED_CONF_APP_HOSTNAME "localhost:3000"

    mbed_trace_init();
    
    myAudio = new MyAudio(mutex_socket, cond_socket, event_queue, socket_buffer, mode);
    myAudio->start();

    myGattClient = new MyGattClient(ble1, event_queue, print_sem, mutex_socket, cond_socket, socket_buffer, ble_changed, ble_buffer, error_ble, LED1, mode);
    myGattClient->start();
    
    mySocket = new MySocket(print_sem, mutex_socket, cond_socket, socket_buffer, mutex_motor, cond_motor, motor_state, ble_changed, ble_buffer, LED3, error_socket, *myGattClient, mode, *myAudio);
    mySocket->start();
    
    
    
    mySensor = new MySensor(mutex_socket, cond_socket, socket_buffer);
    mySensor->start();
    
    myRadar = new MyRadar(mutex_socket, cond_socket, socket_buffer, PA_7, PB_9, PB_8, mode);
    myRadar->start();
    
    myMotorControl = new MyMotorControl(0.8, mutex_motor, cond_motor, motor_state);
    myMotorControl->start();

    
    button.fall(event_queue.event(&error));

    while (true) {
    }
    
    return 0;
}
