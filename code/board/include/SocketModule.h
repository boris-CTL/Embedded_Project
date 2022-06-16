#include "mbed.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"
#include "MyGattClient.h"
#include "MyAudio.h"

#ifndef SOCKET_MODULE_H
#define SOCKET_MODULE_H

#if MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#endif
#endif // MBED_CONF_APP_USE_TLS_SOCKET

class SocketModule{
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;

#if MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    static constexpr size_t REMOTE_PORT = 5000; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    SocketModule(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, Mutex &mutex_motor, ConditionVariable &cond_motor, char &motor_state, bool &changed_ble, char* buffer_ble, MyGattClient &myGattClient, char &mode, MyAudio &myAudio);
    ~SocketModule();
    bool run();
    bool send_http_request(char* buffer);
    bool receive_http_response();
private:
    bool resolve_hostname(SocketAddress &address);
    void wifi_scan();
    void print_network_info();
private:
    NetworkInterface *_net;
    Semaphore *_sem;
    Mutex *_mutex;
    ConditionVariable *_cond;

    Mutex *_mutex_motor;
    ConditionVariable *_cond_motor;
    char *_motor_state;

    // Mutex *_mutex_ble;
    // ConditionVariable *_cond_ble;
    char* _buffer_ble;
    bool* _changed_ble;
    MyGattClient *_myGattClient;

    // bool* _startRadar;
    char* _mode;
    MyAudio *_myAudio;

#if MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

#endif