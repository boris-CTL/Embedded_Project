from email.mime import audio
import socket
from dotenv import load_dotenv
import json
import os
from _thread import *

from MyGesture import MyGesture
from MyDatabase import get_database
from MyDataGUI import MyDataGUI
from MyParsing import parse_data
from MyRadarGUI import MyRadarGUI
from MyAudio import playWav, txt2wav, txt2wav_double

dest = "1111"
device = ""
isJetsonNano = False
mode = ""
audioRecording = False
audioData = ""
audioName = ""

def recvThread(s, myDataGUI, myRadarGUI):
    global device, mode, audioRecording, audioData, audioName
    while True:
        data = s.recv(1024).decode('utf-8')
        # print(data)
        try:
            data = parse_data(data)
            if data["type"]=="ERROR":
                pass
            elif data["type"]=="STM_SENSOR":
                myDataGUI.updateData(data)
            elif data["type"]=="STM_BLE" and mode=="B":
                print("Device: ", device, ", HandleID: ", data["HandleID"], ", Value: ", data["Value"])
            elif data["type"]=="STM_RADAR":
                myRadarGUI.updateData(data["Angle"], data["Dist"])
            elif data["type"]=="STM_HEADER":
                # print("Audio Header Received!")
                audioData = data["data"] + audioData
                audioRecording = False
            elif data["type"]=="STM_AUDIO" and audioRecording:
                # print("Audio Data Received!")
                audioData += data["data"]
            elif data["type"]=="STM_END":
                # print("Audio End!")
                # txt2wav(audioData, audioName)
                # playWav(audioName)
                audioRecording = False
        except:
            pass
        



def cmdThread(s, myGesture, myDataGUI, collection):
    global isJetsonNano, device, mode, audioRecording, audioData, audioName
    isJetsonNano = (input("T: isJetsonNano, Otherwise: PC or RPI: ")=='T')
    while True:
        mode = input("\nMode(G: gesture, D: dataGUI, B: bleControl, R: radar, A: audio): ")
        if mode=="G":
            myGesture.start(isJetsonNano)
        elif mode=="D":
            data = collection.find().sort("_id", 1).limit(50)
            myDataGUI.setData(data)
            myDataGUI.start()
        elif mode=="B":
            # global device
            device = input("Enter your device name: ")
            deviceData = {
                "type": "RPI_BLE",
                "device": device,
                "dest": dest
            }
            s.send(str.encode(json.dumps(deviceData)))
            print("Set ble device to ", device)
            print("Start BLE Monitoring!")
            try:
                while True:
                    pass
            except KeyboardInterrupt:
                mode = ""
        elif mode=="R":
            radarStartSignal = {
                "type": "RPI_RADAR",
                "Start": "T",
                "dest": dest
            }
            s.send(str.encode(json.dumps(radarStartSignal)))
            myRadarGUI.start()
            radarStopSignal = {
                "type": "RPI_RADAR",
                "Start": "F",
                "dest": dest
            }
            s.send(str.encode(json.dumps(radarStopSignal)))
        elif mode=="A":
            audioName = input("Name of your .wav file: ")
            audioStartSignal = {
                "type": "RPI_AUDIO",
                "Start": "T",
                "dest": dest
            }
            s.send(str.encode(json.dumps(audioStartSignal)))
            audioRecording = True
            
            # try:
            #     while True:
            #         pass
            # except KeyboardInterrupt:
            #     pass
            input("Click any key to Pass!")

            mode = ""
            audioStopSignal = {
                "type": "RPI_AUDIO",
                "Start": "F",
                "dest": dest
            }
            s.send(str.encode(json.dumps(audioStopSignal)))
            while audioRecording:
                pass
            txt2wav(audioData, audioName)
            playWav(audioName)
            audioData = ""

            # audioStopSignal = {
            #     "type": "RPI_AUDIO",
            #     "Start": "F",
            #     "dest": dest
            # }
            # s.send(str.encode(json.dumps(audioStopSignal)))

if __name__ == '__main__':
    load_dotenv()
    HOST = os.getenv("SERVER_HOST")
    PORT = int(os.getenv("SERVER_PORT"))

    DB_COLLECTION_NAME = os.getenv("DB_COLLECTION_NAME")
    # Get the database
    DBName = get_database()
    collection = DBName[DB_COLLECTION_NAME]

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    setupData = {
        "type": "RPI_SETUP",
        "ID": "2222"
    }
    s.send(str.encode(json.dumps(setupData)))

    myGesture = MyGesture(s, dest)
    myDataGUI = MyDataGUI()
    myRadarGUI = MyRadarGUI()

    start_new_thread(recvThread, (s, myDataGUI, myRadarGUI))

    cmdThread(s, myGesture, myDataGUI, collection)