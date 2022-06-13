import sys
from MyParsing import parse_data

def run_server(conn, addr, collection):
    print('Connected by', addr)
    conn.send(bytes("Successfully connected.", "utf-8"))
    while True:
        try:
            data = conn.recv(200).decode('utf-8')
            print(sys.getsizeof(data))
            if data[0:8]=='{"type":':
                print(data)
                data = parse_data(data)
                if data["type"]=="STM32_Data":
                    collection.insert_one(data)
                    conn.send(bytes("Data successed!", "utf-8"))
                elif data["type"]=="STM32_BLE":
                    conn.send(bytes("BLE successed!", "utf-8"))
                    continue
                elif data["type"]=="RPI":
                    conn.send(bytes("RPI successed!", "utf-8"))
                    continue
            else:
                print(data)
                conn.send(bytes("BLE Info successed!", "utf-8"))
        except KeyboardInterrupt:
            # quit
            sys.exit()
        except:
            continue
            # print("Error!")

def test_server(conn, addr):
    print('Connected by', addr)
    conn.send(bytes("Successfully connected.", "utf-8"))
    while True:
        try:
            data = conn.recv(200).decode('utf-8')
            print(data)
            conn.send(bytes("Data successed!", "utf-8"))
        except KeyboardInterrupt:
            # quit
            conn.close()
            sys.exit()
        except:
            print("Error!")