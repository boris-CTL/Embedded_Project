import os
import socket
from dotenv import load_dotenv
from _thread import *

from MyServer import test_server

def sensor_data_receiver():
    HOST = os.getenv("SERVER_HOST")
    PORT = int(os.getenv("SERVER_PORT"))

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5) 
        print("Successfully listening.")
        conn, addr = s.accept()

        with conn:
            test_server(conn, addr)

if __name__ == '__main__':
    load_dotenv()
    HOST = os.getenv("SERVER_HOST")
    PORT = int(os.getenv("SERVER_PORT"))
    ThreadCount = 0

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5) 
        print("Successfully listening.")
        
        while True:
            Client, address = s.accept()
            print('Connected to: ' + address[0] + ':' + str(address[1]))
            start_new_thread(test_server, (Client, address))
            ThreadCount += 1
            print('Thread Number: ' + str(ThreadCount))
            
        s.close()
