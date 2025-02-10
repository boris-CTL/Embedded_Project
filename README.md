Digital Signal Processing and Bluetooth Programming for Embedded Devices
===

# Summary
This README provides an overview of the motivation, methodology, results, implementation steps, and references for this embedded project.

![Our Car](./img/OurCar.png)

# Motivation
Inspired by the era of 5G IoT, Mars rovers, and various multifunctional smart home appliances, this project aims to implement a multifunctional smart car using Mbed OS, combined with the multiple sensors and wireless communication modules available on the B-L475E-IOT01A board.

In this project, we extensively utilize features such as Threads, Condition Variables, Mutexes, EventQueues, and IRQs to enhance development efficiency and effectively integrate various functionalities. Additionally, we incorporate advanced techniques related to Digital Signal Processing (DSP).

# Methodology

## Mbed

By defining specific classes and functions for different modules, we effectively integrate synchronous and asynchronous functionalities into a single vehicle.

On the control board side, this project uses `Threads` to separate different functionalities and employs `Condition Variables` to control data transmission via sockets between different `Threads`. The `receive_http_request` function handles the response of different `Threads` upon receiving commands.

The BLE and audio recording functionalities are implemented using separate callback functions. 

Each module is encapsulated within its respective `class` to enhance the efficiency of development and maintenance through object-oriented design.

## Server & Client

Hand gesture recognition is implemented using [Mediapipe](https://github.com/google-ai-edge/mediapipe) and [OpenCV](https://github.com/opencv/opencv). The VideoCapture function on the Jetson Nano requires appropriate configuration to fully leverage GPU performance.

[Pymongo](https://github.com/mongodb/mongo-python-driver) and [Matplotlib](https://github.com/matplotlib/matplotlib) are used for data storage and visualization. Each component executes corresponding tasks via dedicated objects.



## Integration

By establishing socket connections, the server connects the control board and client to configure different functionalities and transmit data. The `memo.json` file contains mappings for various objects and their corresponding data.

# Results
This project successfully implements five sub-functions. The implemented features include:

## 1.Data Visualization
[Demo](https://youtu.be/Mk69kflESu8)

![Data GUI Image](./img/DataGUI.jpg)

## 2.Simple Radar
[Demo](https://youtu.be/RHm4_1YgY9E)

![Radar Image](./img/Radar.jpg)

## 3.Audio Transmission
[Demo](https://youtu.be/UVQB6Vfc58c)

<img src="./img/Audio.png" alt="Audio Image" style="width:200px;"/>

## 4. Gesture-Controlled Remote
[Demo](https://youtu.be/I0MvmhiXyvU)

![Gesture Control Image](./img/GestureControl.jpg)

## 5.BLE Remote Control
[Demo](https://youtu.be/5mQlsj9Lum0)

<img src="./img/BLE.png" alt="BLE Image" style="width:200px;"/>

# How to Implement

## Server:

1. Install all the required python module
2. Create your DB on Mongo Atlas
3. touch `.env` file
```javascipt
DB_PASSWORD="YOUR_DB_PASSWORD"
DB_NAME="YOUR_DB_NAME"
DB_COLLECTION_NAME="YOUR_DB_COLLECTION"
SERVER_HOST="YOUR_SERVER_HOST"
SERVER_PORT=YOUR_SERVER_PORT
```
4. Run `main.py`

## Mbed:

1. Add Library: [mbed-os-ble-utils](https://github.com/ARMmbed/mbed-os-ble-utils#0da6cf31bd6688e4a3ecdf98b356e4767f3248c4)、[wifi-ism43362](https://github.com/ARMmbed/wifi-ism43362/#b8ace72b465870bffbcbe98a411b148b0163a2b1)
2. Modify or create mbed_app.json
3. Set up your WiFi data, server url and port
4. clone this repository and add everything to your project
5. build and run

## Client(PC, RPI, JetsonNano...etc)

1. Set up your os and test your camera
2. Install mediapipe and opencv for gesture recognition and all other python module
3. touch `.env` file
```javascipt
DB_PASSWORD="YOUR_DB_PASSWORD"
DB_NAME="YOUR_DB_NAME"
DB_COLLECTION_NAME="YOUR_DB_COLLECTION"
SERVER_HOST="YOUR_SERVER_HOST"
SERVER_PORT=YOUR_SERVER_PORT
```
4. Run `MyRPIClient.py`
But for RPI3B+, FPS is only about 1.

# References

## Jetson Nano Setup:
https://automaticaddison.com/how-to-set-up-the-nvidia-jetson-nano-developer-kit/

## Raspberry Pi Gesture Recognition:
https://github.com/Kazuhito00/mediapipe-python-sample/blob/main/sample_hand.py
https://zenn.dev/karaage0703/articles/63fed2a261096d
https://github.com/PINTO0309/mediapipe-bin

## WAV File Configuration:
https://onestepcode.com/read-wav-header/