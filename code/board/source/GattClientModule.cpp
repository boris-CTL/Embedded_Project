#include "GattClientModule.h"
#include "dest.h"
#include <cstdio>

GattClientModule::GattClientModule(){
}

GattClientModule::~GattClientModule()
{
    stop();
}

void GattClientModule::setup(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer, bool &isConnected, char &mode){
    _print_sem = &sem;
    _mutex = &mutex;
    _cond = &cond;
    _buffer = buffer;
    _isConnected = &isConnected;
    *_isConnected = false;
    _mode = &mode;
}

void GattClientModule::start(BLE &ble, events::EventQueue &event_queue)
{
    _ble = &ble;
    _event_queue = &event_queue;
    _client = &_ble->gattClient();
}

void GattClientModule::start_discovery(BLE &ble_interface, events::EventQueue &event_queue, const ble::ConnectionCompleteEvent &event)
{
    _connection_handle = event.getConnectionHandle();

    // setup the event handlers called during the process
    _client->onDataWritten().add(as_cb(&Self::when_descriptor_written));
    _client->onHVX().add(as_cb(&Self::when_characteristic_changed));

    // The discovery process will invoke when_service_discovered when a
    // service is discovered, when_characteristic_discovered when a
    // characteristic is discovered and when_service_discovery_ends once the
    // discovery process has ended.
    _client->onServiceDiscoveryTermination(as_cb(&Self::when_service_discovery_ends));
    ble_error_t error = _client->launchServiceDiscovery(
        _connection_handle,
        as_cb(&Self::when_service_discovered),
        as_cb(&Self::when_characteristic_discovered)
    );

    if (error) {
        _print_sem->acquire();
        printf("Error %u returned by _client->launchServiceDiscovery.\r\n", error);
        _print_sem->release();
        return;
    }

    // register as a handler for GattClient events
    _client->setEventHandler(this);

    // this might not result in a new value but if it does we will be informed through
    // an call in the event handler we just registered
    _client->negotiateAttMtu(_connection_handle);

    _print_sem->acquire();
    printf("Client process started: initiate service discovery.\r\n");
    _print_sem->release();
    *_isConnected = true;
}

void GattClientModule::stop()
{
    if (!_client) {
        return;
    }

    // unregister event handlers
    _client->onDataWritten().detach(as_cb(&Self::when_descriptor_written));
    _client->onHVX().detach(as_cb(&Self::when_characteristic_changed));
    _client->onServiceDiscoveryTermination(nullptr);

    // remove discovered characteristics
    clear_characteristics();

    // clean up the instance
    _connection_handle = 0;
    _characteristics = nullptr;
    _it = nullptr;
    _descriptor_handle = 0;

    _print_sem->acquire();
    printf("Client process stopped.\r\n");
    _print_sem->release();
}

void GattClientModule::onAttMtuChange(ble::connection_handle_t connectionHandle, uint16_t attMtuSize)
{
    _print_sem->acquire();
    printf(
        "ATT_MTU changed on the connection %d to a new value of %d.\r\n",
        connectionHandle,
        attMtuSize
        /* maximum size of an attribute written in a single operation is one less */
    );
    _print_sem->release();
}

void GattClientModule::when_service_discovered(const DiscoveredService *discovered_service)
{
    // _print_sem->acquire();
    // print information of the service discovered
    // printf("Service discovered: value = ");
    // print_uuid(discovered_service->getUUID());
    // printf(", start = %u, end = %u.\r\n",
    //     discovered_service->getStartHandle(),
    //     discovered_service->getEndHandle()
    // );
    // _print_sem->release();
}

void GattClientModule::when_characteristic_discovered(const DiscoveredCharacteristic *discovered_characteristic)
{
    // print characteristics properties
    // printf("\tCharacteristic discovered: uuid = ");
    // print_uuid(discovered_characteristic->getUUID());
    // printf(", properties = ");
    // print_properties(discovered_characteristic->getProperties());
    // printf(
    //     ", decl handle = %u, value handle = %u, last handle = %u.\r\n",
    //     discovered_characteristic->getDeclHandle(),
    //     discovered_characteristic->getValueHandle(),
    //     discovered_characteristic->getLastHandle()
    // );
    // add the characteristic into the list of discovered characteristics
    bool success = add_characteristic(discovered_characteristic);
    if (!success) {
        _print_sem->acquire();
        printf("Error: memory allocation failure while adding the discovered characteristic.\r\n");
        _print_sem->release();  
        _client->terminateServiceDiscovery();
        stop();
        return;
    }
}

void GattClientModule::when_service_discovery_ends(ble::connection_handle_t connection_handle)
{
    if (!_characteristics) {
        _print_sem->acquire();
        printf("No characteristics discovered, end of the process.\r\n");
        _print_sem->release();
        return;
    }

    // printf("All services and characteristics discovered, process them.\r\n");

    // reset iterator and start processing characteristics in order
    _it = nullptr;
    _event_queue->call(mbed::callback(this, &Self::process_next_characteristic));
}

void GattClientModule::process_next_characteristic(void)
{
    if (!_it) {
        _it = _characteristics;
    } else {
        _it = _it->next;
    }

    while (_it) {
        Properties_t properties = _it->value.getProperties();

        if (properties.read()) {
            read_characteristic(_it->value);
            return;
        } else if(properties.notify() || properties.indicate()) {
            discover_descriptors(_it->value);
            return;
        } else {
            // _print_sem->acquire();
            // printf(
            //     "Skip processing of characteristic %u\r\n",
            //     _it->value.getValueHandle()
            // );
            // _print_sem->release();
            _it = _it->next;
        }
    }
    // _print_sem->acquire();
    // printf("All characteristics discovered have been processed.\r\n");
    // _print_sem->release();
}

void GattClientModule::read_characteristic(const DiscoveredCharacteristic &characteristic)
{
    // _print_sem->acquire();
    // printf("Initiating read at %u.\r\n", characteristic.getValueHandle());
    // _print_sem->release();

    ble_error_t error = characteristic.read(0, as_cb(&Self::when_characteristic_read));

    if (error) {
        _print_sem->acquire();
        printf(
            "Error: cannot initiate read at %u due to %u\r\n",
            characteristic.getValueHandle(), error
        );
        _print_sem->release();   
        stop();
    }
}

void GattClientModule::when_characteristic_read(const GattReadCallbackParams *read_event)
{
    // _print_sem->acquire();
    // printf("\tCharacteristic value at %u equal to: ", read_event->handle);
    // for (size_t i = 0; i <  read_event->len; ++i) {
    //     printf("0x%02X ", read_event->data[i]);
    // }
    // printf(".\r\n");
    // _print_sem->release();

    Properties_t properties = _it->value.getProperties();

    if(properties.notify() || properties.indicate()) {
        discover_descriptors(_it->value);
    } else {
        process_next_characteristic();
    }
}

void GattClientModule::discover_descriptors(const DiscoveredCharacteristic &characteristic)
{
    // printf("Initiating descriptor discovery of %u.\r\n", characteristic.getValueHandle());

    _descriptor_handle = 0;
    ble_error_t error = characteristic.discoverDescriptors(
        as_cb(&Self::when_descriptor_discovered),
        as_cb(&Self::when_descriptor_discovery_ends)
    );

    if (error) {
        _print_sem->acquire();
        printf(
            "Error: cannot initiate discovery of %04X due to %u.\r\n",
            characteristic.getValueHandle(), error
        );
        _print_sem->release();  
        stop();
    }
}

void GattClientModule::when_descriptor_discovered(const DiscoveryCallbackParams_t* event)
{
    // printf("\tDescriptor discovered at %u, UUID: ", event->descriptor.getAttributeHandle());
    // print_uuid(event->descriptor.getUUID());
    // printf(".\r\n");

    if (event->descriptor.getUUID() == BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG) {
        _descriptor_handle = event->descriptor.getAttributeHandle();
        _client->terminateCharacteristicDescriptorDiscovery(
            event->characteristic
        );
    }
}

void GattClientModule::when_descriptor_discovery_ends(const TerminationCallbackParams_t *event)
{
    // shall never happen but happen with android devices ...
    // process the next charateristic
    if (!_descriptor_handle) {
        // _print_sem->acquire();
        // printf("\tWarning: characteristic with notify or indicate attribute without CCCD.\r\n");
        // _print_sem->release();   
        process_next_characteristic();
        return;
    }

    Properties_t properties = _it->value.getProperties();

    uint16_t cccd_value =
        (properties.notify() << 0) | (properties.indicate() << 1);

    ble_error_t error = _client->write(
        GattClient::GATT_OP_WRITE_REQ,
        _connection_handle,
        _descriptor_handle,
        sizeof(cccd_value),
        reinterpret_cast<uint8_t*>(&cccd_value)
    );

    if (error) {
        _print_sem->acquire();
        printf(
            "Error: cannot initiate write of CCCD %u due to %u.\r\n",
            _descriptor_handle, error
        );
        _print_sem->release();    
        stop();
    }
}

void GattClientModule::when_descriptor_written(const GattWriteCallbackParams* event)
{
    // should never happen
    if (!_descriptor_handle) {
        _print_sem->acquire();
        printf("\tError: received write response to unsolicited request.\r\n");
        _print_sem->release();
        stop();
        return;
    }
    // _print_sem->acquire();
    // printf("\tCCCD at %u written.\r\n", _descriptor_handle);
    // _print_sem->release();
    
    _descriptor_handle = 0;
    process_next_characteristic();
}

void GattClientModule::when_characteristic_changed(const GattHVXCallbackParams* event)
{
    if(*_mode=='N'){
        printf("Character Changed!\n");
        _mutex->lock();
        // _print_sem->acquire();
        int len = sprintf(_buffer, "{\"type\":\"STM_BLE\", \"HandleID\":%u, \"Value\":\"0x", event->handle);
        // printf("Change on attribute %u: new value = ", event->handle);
        for (size_t i = 0; i < event->len; ++i) {
            if(len<200){
                len += sprintf(_buffer+len, "%02X", event->data[i]);
            }
            // printf("0x%02X ", event->data[i]);
        }
        len += sprintf(_buffer+len, "\", \"dest\":\"%s\"}", DEST);
        // printf(".\r\n");
        // _print_sem->release();
        _cond->notify_all();
        _mutex->unlock();
        ThisThread:wait_us(1000);
    }
}

bool GattClientModule::add_characteristic(const DiscoveredCharacteristic *characteristic)
{
    DiscoveredCharacteristicNode* new_node =
        new(std::nothrow) DiscoveredCharacteristicNode(*characteristic);

    if (new_node == nullptr) {
        // _print_sem->acquire();
        // printf("Error while allocating a new characteristic.\r\n");
        // _print_sem->release();
        return false;
    }

    if (_characteristics == nullptr) {
        _characteristics = new_node;
    } else {
        DiscoveredCharacteristicNode* c = _characteristics;
        while(c->next) {
            c = c->next;
        }
        c->next = new_node;
    }

    return true;
}

void GattClientModule::clear_characteristics(void)
{
    DiscoveredCharacteristicNode *c= _characteristics;

    while (c) {
        DiscoveredCharacteristicNode *n = c->next;
        delete c;
        c = n;
    }
}

void GattClientModule::print_uuid(const UUID &uuid)
{
    const uint8_t *uuid_value = uuid.getBaseUUID();
    
    // UUIDs are in little endian, print them in big endian
    // for (size_t i = 0; i < uuid.getLen(); ++i) {
    //     printf("%02X", uuid_value[(uuid.getLen() - 1) - i]);
    // }
}

void GattClientModule::print_properties(const Properties_t &properties)
{
    const struct {
        bool (Properties_t::*fn)() const;
        const char* str;
    } prop_to_str[] = {
        { &Properties_t::broadcast, "broadcast" },
        { &Properties_t::read, "read" },
        { &Properties_t::writeWoResp, "writeWoResp" },
        { &Properties_t::write, "write" },
        { &Properties_t::notify, "notify" },
        { &Properties_t::indicate, "indicate" },
        { &Properties_t::authSignedWrite, "authSignedWrite" }
    };
    
    // printf("[");
    // for (size_t i = 0; i < (sizeof(prop_to_str) / sizeof(prop_to_str[0])); ++i) {
    //     if ((properties.*(prop_to_str[i].fn))()) {
    //         printf(" %s", prop_to_str[i].str);
    //     }
    // }
    // printf(" ]");
}