import socket

HOST = '192.168.50.197'
PORT = 3000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

while True:
    cmd = input("Please input msg:")
    s.send(cmd)
    data = s.recv(1024)
    print("server send : %s " % (data))