#include "SocketModule.h"
#include <cstdio>
#include <cstring>

SocketModule::SocketModule(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, Mutex &mutex_motor, ConditionVariable &cond_motor, char &motor_state, bool &changed_ble, char* buffer_ble, MyGattClient &myGattClient, char &mode, MyAudio &myAudio):
    _net(NetworkInterface::get_default_instance()), _sem(&sem), _mutex_motor(&mutex_motor), _cond_motor(&cond_motor), _motor_state(&motor_state), _changed_ble(&changed_ble), _buffer_ble(buffer_ble), _myGattClient(&myGattClient), _mode(&mode), _myAudio(&myAudio){
}

SocketModule::~SocketModule(){
    if (_net) {
        _net->disconnect();
    }
}

bool SocketModule::run(){
    if (!_net) {
        _sem->acquire();
        printf("Error! No network interface found.\r\n");
        _sem->release();
        return false;
    }

    /* if we're using a wifi interface run a quick scan */
    if (_net->wifiInterface()) {
        /* the scan is not required to connect and only serves to show visible access points */
        // _net->wifiInterface();
        wifi_scan();

        /* in this example we use credentials configured at compile time which are used by
            * NetworkInterface::connect() but it's possible to do this at runtime by using the
            * WiFiInterface::connect() which takes these parameters as arguments */
    }

    /* connect will perform the action appropriate to the interface type to connect to the network */
    _sem->acquire();
    printf("Connecting to the network...\r\n");
    _sem->release();

    nsapi_size_or_error_t result = _net->connect();
    if (result != 0) {
        _sem->acquire();
        printf("Error! _net->connect() returned: %d\r\n", result);
        _sem->release();
        return false;
    }

    print_network_info();

    /* opening the socket only allocates resources */
    result = _socket.open(_net);
    if (result != 0) {
        _sem->acquire();
        printf("Error! _socket.open() returned: %d\r\n", result);
        _sem->release();
        return false;
    }

#if MBED_CONF_APP_USE_TLS_SOCKET
    result = _socket.set_root_ca_cert(root_ca_cert);
    if (result != NSAPI_ERROR_OK) {
        _sem->acquire();
        printf("Error: _socket.set_root_ca_cert() returned %d\n", result);
        _sem->release();
        return false;
    }
    _socket.set_hostname(MBED_CONF_APP_HOSTNAME);
#endif // MBED_CONF_APP_USE_TLS_SOCKET

    /* now we have to find where to connect */

    SocketAddress address;

    if (!resolve_hostname(address)) {
        return false;
    }

    address.set_port(REMOTE_PORT);

    /* we are connected to the network but since we're using a connection oriented
        * protocol we still need to open a connection on the socket */
    _sem->acquire();
    printf("Opening connection to remote port %d\r\n", REMOTE_PORT);
    _sem->release();

    result = _socket.connect(address);
    if (result != 0) {
        _sem->acquire();
        printf("Error! _socket.connect() returned: %d\r\n", result);
        _sem->release();
        return false;
    }
    return true;
}

bool SocketModule::resolve_hostname(SocketAddress &address){
    const char hostname[] = MBED_CONF_APP_HOSTNAME;

    /* get the host address */
    _sem->acquire();
    printf("\nResolve hostname %s\r\n", hostname);
    _sem->release();
    nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
    if (result != 0) {
        _sem->acquire();
        printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
        _sem->release();
        return false;
    }

    _sem->acquire();
    printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );
    _sem->release();

    return true;
}

bool SocketModule::send_http_request(char* buffer)
{
    /* loop until whole request sent */
    // const char buffer[] = "GET / HTTP/1.1\r\n"
    //                         "Host: our server url\r\n"
    //                         "Connection: close\r\n"
    //                         "\r\n";

    nsapi_size_t bytes_to_send = strlen(buffer);
    nsapi_size_or_error_t bytes_sent = 0;

    // _sem->acquire();
    // printf("\r\nSending message: \r\n%s", buffer);
    // _sem->release();

    while (bytes_to_send) {
        bytes_sent = _socket.send(buffer + bytes_sent, bytes_to_send);
        if (bytes_sent < 0) {
            _sem->acquire();
            printf("Error! _socket.send() returned: %d\r\n", bytes_sent);
            _sem->release();
            return false;
        } else {
            _sem->acquire();
            // printf("Sent %d bytes\r\n", bytes_sent);
            _sem->release();
        }

        bytes_to_send -= bytes_sent;
    }

    // printf("Complete message sent\r\n");

    return true;
}

bool SocketModule::receive_http_response()
{   
    // while (true) {

    char buffer[MAX_MESSAGE_RECEIVED_LENGTH];
    int remaining_bytes = MAX_MESSAGE_RECEIVED_LENGTH;
    int received_bytes = 0;

    nsapi_size_or_error_t result = remaining_bytes;
    
    result = _socket.recv(buffer + received_bytes, remaining_bytes);
    // printf("Result: %d", result);
    if (result < 0) {
        _sem->acquire();
        printf("Error! _socket.recv() returned: %d\r\n", result);
        _sem->release();
        return false;
    }
    received_bytes += result;
    remaining_bytes -= result;
    if(result!=0){
        _sem->acquire();
        // printf("received %d bytes:\r\n%.*s\r\n\r\n", received_bytes, strstr(buffer, "\n") - buffer, buffer);
        // printf("received %d bytes:\r\n%.*s\r\n", received_bytes, result, buffer);
        printf("received %d bytes:\r\n%s\r\n", received_bytes, buffer);
        _sem->release();

        if(strstr(buffer, "Go")){
            _mutex_motor->lock();
            *_motor_state = 'G';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "Stop")){
            _mutex_motor->lock();
            *_motor_state = 'S';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "Left")){
            _mutex_motor->lock();
            *_motor_state = 'L';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "Right")){
            _mutex_motor->lock();
            *_motor_state = 'R';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "Front")){
            _mutex_motor->lock();
            *_motor_state = 'F';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "Back")){
            _mutex_motor->lock();
            *_motor_state = 'B';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "SpeedUp")){
            _mutex_motor->lock();
            *_motor_state = 'U';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "SlowDown")){
            _mutex_motor->lock();
            *_motor_state = 'D';
            _cond_motor->notify_all();
            _mutex_motor->unlock();
        }
        else if(strstr(buffer, "Bluetooth:")){
            // _mutex_ble->lock();
            int count = 10;
            while ((count<MAX_MESSAGE_RECEIVED_LENGTH)&&(buffer[count]!='\n')){
                count++;
            }
            // count -= 10;
            printf("Count: %d\n", count);
            // delete _buffer_ble;
            // _buffer_ble = new char[count];
            // char* device = new char[count+1];
            // char device[count];
            // for(int i=10, j=0; j<count; i++, j++){
            //     _buffer_ble[j] = buffer[i];
            //     // device[j] = buffer[i];
            // }
            string device(buffer+10, buffer+count);
            // strcpy(_buffer_ble, device.c_str()); 
            // char newDevicePointer[newDevice.length() + 1]; 
            // strcpy(newDevicePointer, newDevice.c_str()); 
            // printf("BLE Received: %s, %d\n", _buffer_ble, strlen(_buffer_ble));
            printf("BLE Received: %s, %d\n", device.c_str(), strlen(device.c_str()));
            delete _buffer_ble;
            _buffer_ble = new char[strlen(device.c_str())];
            strcpy(_buffer_ble, device.c_str());
            printf("BLE Received Char: %s, %d\n", _buffer_ble, strlen(_buffer_ble));
            _myGattClient->modify_BLE_device(_buffer_ble);
            // printf("BLE Received: %s, %d\n", _buffer_ble, count);
            // *_changed_ble = true;
            // _cond_ble->notify_all();
            // _mutex_ble->unlock();
        }
        else if(strstr(buffer, "RadarStart")){
            *_mode = 'R';
        }
        else if(strstr(buffer, "RadarHalt")){
            *_mode = 'N';
        }
        else if(strstr(buffer, "AudioStart")){
            _myAudio->start_recording();
            *_mode = 'A';
        }
        else if(strstr(buffer, "AudioHalt")){
            _myAudio->stop_recording();
            *_mode = 'N';
        }
    }
    // }
    
    return true;
}

void SocketModule::wifi_scan()
{
    WiFiInterface *wifi = _net->wifiInterface();

    WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

    /* scan call returns number of access points found */
    int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

    if (result <= 0) {
        _sem->acquire();
        printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
        _sem->release();
        return;
    }

    _sem->acquire();
    printf("%d networks available:\r\n", result);
    _sem->release();

    _sem->acquire();
    for (int i = 0; i < result; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                ap[i].get_rssi(), ap[i].get_channel());  
    }
    printf("\r\n");
    _sem->release();
}

void SocketModule::print_network_info()
{
    /* print the network info */
    SocketAddress a;
    _net->get_ip_address(&a);
    _sem->acquire();
    printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    _sem->release();
    
    _net->get_netmask(&a);
    _sem->acquire();
    printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    _sem->release();
    
    _net->get_gateway(&a);
    _sem->acquire();
    printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    _sem->release();
}