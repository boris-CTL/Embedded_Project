import socket
from dotenv import load_dotenv
import json
import os
import sys
from _thread import *

dest = "1111"

def recvThread(s):
    while True:
        response = s.recv(1024)
        print(response.decode('utf-8'))

if __name__ == '__main__':
    load_dotenv()
    HOST = os.getenv("SERVER_HOST")
    PORT = int(os.getenv("SERVER_PORT"))

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))

    setupData = {
        "type": "STM_SETUP",
        "ID": "2222"
    }
    s.send(str.encode(json.dumps(setupData)))
    
    start_new_thread(recvThread, tuple([s]))

    while True:
        try:
            continue
        except KeyboardInterrupt:
            # quit
            sys.exit()
        except Exception as e:
            # continue
            # # print("Error!")
            print(e)
            break
        

    # dest = "1111"
    # while True:
    #     cmd = input("Please input msg:")
    #     data = {
    #         "type": "STM_MSG",
    #         "msg": str(cmd),
    #         "dest": dest
    #     }
    #     s.send(bytes(json.dumps(data),encoding="utf-8"))
    #     response = s.recv(1024)
    #     print(response.decode('utf-8'))