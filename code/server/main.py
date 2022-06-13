import os
import socket
from dotenv import load_dotenv
import sys
import json
import time

from _thread import *

from MyDatabase import get_database
from MyParsing import parse_data

# Command to get ip address: ipconfig getifaddr en0

if __name__ == '__main__':
    load_dotenv()
    DB_COLLECTION_NAME = os.getenv("DB_COLLECTION_NAME")
    # Get the database
    DBName = get_database()
    collectionName = DBName[DB_COLLECTION_NAME]
    # sensor_data_receiver()

    HOST = os.getenv("SERVER_HOST")
    PORT = int(os.getenv("SERVER_PORT"))
    ThreadCount = 0

    addrDict = {}

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5) 
        print("Successfully listening.")
        
        def socketProcessing(conn, addr, collection, s):
            while True:
                try:
                    data = conn.recv(256).decode('utf-8')
                    if data[0:8]=='{"type":':
                        print(data)
                        data = parse_data(data)
                        if data["type"]=="RPI_SETUP":
                            addrDict.update({data["ID"]: conn})
                        elif data["type"]=="STM_SETUP":
                            addrDict.update({data["ID"]: conn})
                        else:
                            errorData = {
                                "type": "ERROR",
                                "msg": "Client Not Found!"
                            }
                            if data["type"]=="RPI_GES":
                                if data["dest"] in addrDict:
                                    if not(data["left"] == "None"):
                                        addrDict[data["dest"]].send(bytes(data["left"], "utf-8"))
                                    else:
                                        addrDict[data["dest"]].send(bytes(data["right"], "utf-8"))
                                    # addrDict[data["dest"]].send(bytes(json.dumps(data), "utf-8"))
                                else:
                                    conn.send(bytes(json.dumps(errorData), "utf-8"))
                            elif data["type"]=="STM_SENSOR":
                                print("Sensor Data In!")
                                data["time"] = time.time()
                                if data["dest"] in addrDict:
                                    addrDict[data["dest"]].send(bytes(json.dumps(data), "utf-8"))
                                else:
                                    conn.send(bytes(json.dumps(errorData), "utf-8"))
                                collection.insert_one(data)
                            elif data["type"]=="STM_BLE":
                                if data["dest"] in addrDict:
                                    addrDict[data["dest"]].send(bytes(json.dumps(data), "utf-8"))
                                else:
                                    conn.send(bytes(json.dumps(errorData), "utf-8"))
                            elif data["type"]=="RPI_BLE":
                                if data["dest"] in addrDict:
                                    addrDict[data["dest"]].send(bytes(("Bluetooth:"+data["device"]+"\n"), "utf-8"))
                                else:
                                    conn.send(bytes(json.dumps(errorData), "utf-8"))
                except KeyboardInterrupt:
                    # quit
                    conn.close()
                    sys.exit()
                except Exception as e:
                    # continue
                    # # print("Error!")
                    print(e)
                    break
        
        while True:
            Client, address = s.accept()
            print('Connected to: ' + address[0] + ':' + str(address[1]))
            start_new_thread(socketProcessing, (Client, address, collectionName, s))
            ThreadCount += 1
            print('Thread Number: ' + str(ThreadCount))