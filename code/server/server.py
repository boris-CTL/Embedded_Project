# import socket
# import json
# import datetime as dt
# import matplotlib.pyplot as plt
# import matplotlib.animation as animation
# from collections import deque
# from multiprocessing import Process, Array
# import time
from concurrent.futures import thread
import socket
import threading
import time
import sys

from sqlalchemy import null

# Command to get ip address: ipconfig getifaddr en0
HOST = "192.168.50.197"
PORT = 3000

def sensor_data_receiver():
    HOST = "192.168.50.197"
    PORT = 3000

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen() 
        conn, addr = s.accept()

        with conn:
            print('Connected by', addr)
            while True:
                sensor_data = conn.recv(200).decode('utf-8')
                print(sys.getsizeof(sensor_data))                 
                print('Received from socket server : ', sensor_data)

if __name__ == '__main__':
    sensor_data_receiver()