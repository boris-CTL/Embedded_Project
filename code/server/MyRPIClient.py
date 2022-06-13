import socket
from dotenv import load_dotenv
import json
import os
from _thread import *

from MyGesture import MyGesture
from MyDatabase import get_database
from MyDataGUI import MyDataGUI
from MyParsing import parse_data

dest = "1111"
device = ""
isJetsonNano = False
mode = ""

def recvThread(s, myDataGUI):
    while True:
        data = s.recv(1024)
        data = parse_data(data)
        if data["type"]=="ERROR":
            pass
        elif data["type"]=="STM_SENSOR":
            myDataGUI.updateData(data)
        elif data["type"]=="STM_BLE" and mode=="B":
            print("Device: ", device, ", HandleID: ", data["HandleID"], ", Value: ", data["Value"])



def cmdThread(s, myGesture, myDataGUI, collection):
    global isJetsonNano, device, mode
    isJetsonNano = (input("T: isJetsonNano, Otherwise: PC or RPI: ")=='T')
    while True:
        mode = input("\nMode(G: gesture, D: dataGUI, B: bleControl): ")
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

    start_new_thread(recvThread, (s, myDataGUI))

    cmdThread(s, myGesture, myDataGUI, collection)