import socket
from dotenv import load_dotenv
import json
import os
import sys
from _thread import *
# import multiprocessing
import threading

from MyGesture import MyGesture
from MyDatabase import get_database
from MyDataGUI import MyDataGUI
from MyParsing import parse_data

dest = "1111"

def recvThread(s, myDataGUI):
    while True:
        data = s.recv(1024)
        # print(response.decode('utf-8'))
        data = parse_data(data)
        if data["type"]=="ERROR":
            # print(data["msg"])
            pass
        elif data["type"]=="STM_SENSOR":
            myDataGUI.updateData(data)



def cmdThread(s, myGesture, myDataGUI, collection):
#     myGesture = MyGesture(s, dest)
#     myDataGUI = MyDataGUI(collection)
#     myDataGUI.getData()

    while True:
        mode = input("Mode(G: gesture, D: dataGUI): ")
        if mode=="G":
            myGesture.start()
        elif mode=="D":
            data = collection.find().sort("_id", 1).limit(50)
            myDataGUI.setData(data)
            myDataGUI.start()

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
    # start_new_thread(cmdThread, tuple([s]))

    # while True:
    #     continue

    cmdThread(s, myGesture, myDataGUI, collection)
    # try:
    #     cmdThread(s, collection)
    # except KeyboardInterrupt:
    #     # quit
    #     sys.exit()
    # except Exception as e:
    #     print(e)
    
    # while True:
    #     cmd = input("Please input msg:")
    #     data = {
    #         "type": "RPI_MSG",
    #         "msg": str(cmd),
    #         "dest": dest
    #     }
    #     s.send(bytes(json.dumps(data),encoding="utf-8"))
    #     response = s.recv(1024)
    #     print(response.decode('utf-8'))